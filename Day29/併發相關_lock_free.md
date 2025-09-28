# 併發相關 lock free

今天就接著昨天thread pool的項目，來探討`lock free`的部分，也就是我們能將任務隊列 queue 改以lock free的方式實現。

## lock free queue

```cpp
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>

template<typename T>
class MPMCQueue {
public:
    explicit MPMCQueue(size_t capacity)
    {
        // 容量取 2 的冪次（演算法需要），最小 2
        size_ = 1;
        while (size_ < capacity) size_ <<= 1;
        mask_ = size_ - 1;

        buffer_.resize(size_);
        for (size_t i = 0; i < size_; ++i) {
            buffer_[i].seq.store(i, std::memory_order_relaxed);
        }
        enqueue_pos_.store(0, std::memory_order_relaxed);
        dequeue_pos_.store(0, std::memory_order_relaxed);
    }

    // MPMC 無鎖入列；成功回 true，滿了回 false
    template<class U>
    bool enqueue(U&& v) {
        Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            size_t seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)pos;
            if (dif == 0) {
                if (enqueue_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed))
                {
                    break; // 取得該格
                }
            } else if (dif < 0) {
                return false; // 滿
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
        // 寫入資料
        cell->storage = std::forward<U>(v);
        // 發佈：把 seq 設為 pos+1，讓消費者可見
        cell->seq.store(pos + 1, std::memory_order_release);
        return true;
    }

    // MPMC 無鎖出列；成功回 true，把元素移到 out，空回 false
    bool dequeue(T& out) {
        Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            size_t seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
            if (dif == 0) {
                if (dequeue_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed))
                {
                    break; // 取得該格
                }
            } else if (dif < 0) {
                return false; // 空
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
        // 取出資料
        out = std::move(cell->storage);
        // 重置此格的 seq，讓下一輪生產者可以使用
        cell->seq.store(pos + mask_ + 1, std::memory_order_release);
        return true;
    }

    // 非嚴格；僅供收尾判斷參考
    bool likely_empty() const {
        return enqueue_pos_.load(std::memory_order_relaxed) ==
               dequeue_pos_.load(std::memory_order_relaxed);
    }

private:
    struct Cell {
        std::atomic<size_t> seq;
        T storage;
    };

    std::vector<Cell> buffer_;
    size_t size_{0};
    size_t mask_{0};
    std::atomic<size_t> enqueue_pos_{0};
    std::atomic<size_t> dequeue_pos_{0};
};

class counting_semaphore {
public:
    explicit counting_semaphore(ptrdiff_t init = 0) : count_(init) {}
    void release(ptrdiff_t n = 1) {
        std::unique_lock<std::mutex> lk(m_);
        count_ += n;
        for (ptrdiff_t i = 0; i < n; ++i) cv_.notify_one();
    }
    void acquire() {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]{ return count_ > 0; });
        --count_;
    }
private:
    std::mutex m_;
    std::condition_variable cv_;
    ptrdiff_t count_;
};

class ThreadPoolLF {
public:
    explicit ThreadPoolLF(unsigned n = std::thread::hardware_concurrency(),
                          size_t queue_capacity = 1 << 16 /* 65536 */)
        : stop_(false), tasks_(queue_capacity)
    {
        if (n == 0) n = 1;
        workers_.reserve(n);
        try {
            for (unsigned i = 0; i < n; ++i) {
                workers_.emplace_back([this]{ this->worker_loop(); });
            }
        } catch (...) {
            stop_.store(true, std::memory_order_relaxed);
            // 喚醒所有 worker 退出
            items_.release((ptrdiff_t)workers_.size());
            for (auto& t : workers_) if (t.joinable()) t.join();
            throw;
        }
    }

    ~ThreadPoolLF() { Stop(); }

    ThreadPoolLF(const ThreadPoolLF&) = delete;
    ThreadPoolLF& operator=(const ThreadPoolLF&) = delete;

    template<class F>
    void Submit(F&& f) {
        if (stop_.load(std::memory_order_acquire)) {
            throw std::runtime_error("ThreadPool stopped");
        }
        // 無鎖入列；如滿則做輕微退讓（可改策略：阻塞/丟棄/擴容）
        std::function<void()> fn(std::forward<F>(f));
        while (!tasks_.enqueue(std::move(fn))) {
            if (stop_.load(std::memory_order_acquire))
                throw std::runtime_error("ThreadPool stopped");
            std::this_thread::yield(); // backoff
        }
        // 有新任務 → 發一個 permit
        items_.release(1);
    }

    void Stop() {
        bool expected = false;
        if (!stop_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
            return; // 已停

        // 喚醒所有 worker，讓他們檢查 stop_ 並退場
        items_.release((ptrdiff_t)workers_.size());
        for (auto& t : workers_) if (t.joinable()) t.join();
        workers_.clear();
    }

private:
    void worker_loop() {
        for (;;) {
            // 等待有「可取的項目」或收到停止信號
            items_.acquire();

            if (stop_.load(std::memory_order_acquire)) {
                // 若已停止，但佇列裡可能還有剩餘任務 → 嘗試清空它
                std::function<void()> task;
                while (tasks_.dequeue(task)) {
                    safe_run(task);
                }
                return; // 退出
            }

            // 取一個任務（理論上 acquire 之後應該有；但保險起見 loop）
            std::function<void()> task;
            if (tasks_.dequeue(task)) {
                safe_run(task);
            } else {
                // 極端競爭下可能出現「被別的 worker 先取走」→ 繼續下一輪
                continue;
            }
        }
    }

    static void safe_run(std::function<void()>& task) noexcept {
        try { task(); } catch (...) {
            // 生產用可以放 logging；benchmark 時直接吞掉
        }
    }

    std::vector<std::thread> workers_;
    std::atomic<bool> stop_;
    MPMCQueue<std::function<void()>> tasks_;  // ← 無鎖佇列
    counting_semaphore items_{0};             // ← 可用項目數（阻塞等待）
};

class CountDownLatch
{
public:
    explicit CountDownLatch(int count) : count_(count) {}
    void count_down()
    {
        std::unique_lock<std::mutex> lk(m_);
        if (--count_ == 0)
            cv_.notify_all();
    }
    void wait()
    {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]
                 { return count_ == 0; });
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    int count_;
};

std::string FormatTimeNow(const char *format)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);

    char buf[20];
    std::strftime(buf, sizeof(buf), format, now_tm);
    return std::string(buf);
}

std::mutex io_mtx;

void Task()
{
    std::thread::id thread_id = std::this_thread::get_id();
    std::string now = FormatTimeNow("%H:%M:%S");
    {
        std::lock_guard<std::mutex> lock(io_mtx);
        std::cout << "Thread " << thread_id << ": wait succeeded"
                  << " (" << now << ")" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    std::cout << "thread number: " << std::thread::hardware_concurrency() << std::endl;
    ThreadPoolBetter thread_pool;
    int tasks = 100000;
    CountDownLatch latch((int)tasks);
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < tasks; ++i)
    {
        thread_pool.Submit([&latch]
                           { Task(); latch.count_down(); });
    }
    latch.wait();
    auto end = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double tps = (ms == 0) ? 0.0 : (1000.0 * tasks / ms);

    std::cout << "tasks=" << tasks
              << " time=" << ms << " ms"
              << " throughput=" << (uint64_t)tps << " tasks/s\n";

    return 0;
}
```

## 測試結果

出乎意料的是lock free，並沒有比較快。

```sh
# thread pool with mutex
tasks=100000 time=5947 ms throughput=16815 tasks/s

# thread pool with lock free
tasks=100000 time=6079 ms throughput=16450 tasks/s
```

## 第三方庫的lock free queue

基於前面的lock free是由AI生成的，這邊就再透過比較知名的第三方庫來比較一下性能

- [concurrentqueue](https://github.com/cameron314/concurrentqueue)
- [MPMCQueue](https://github.com/rigtorp/MPMCQueue)

```sh
vcpkg install concurrentqueue mpmcqueue
```

## 使用套件

這邊來調整一下整個專案，因為改用了vcpkg，cmake要重新加上vcpkg的toolchain

```sh
rm -fr build # 移除上次建置資料夾
# 使用下面的指令，vcpkg就會幫我們跟cmake做第三方套件整合
cmake -B [build directory] -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake 
```

- 使用concurrentqueue

```cpp
// ThreadPoolMoody.h
#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <stdexcept>

#include <moodycamel/blockingconcurrentqueue.h> 

class ThreadPoolMoody {
public:
    explicit ThreadPoolMoody(unsigned n = std::thread::hardware_concurrency())
        : stop_(false) {
        if (n == 0) n = 1;
        workers_.reserve(n);
        try {
            for (unsigned i = 0; i < n; ++i) {
                workers_.emplace_back([this]{ worker_loop(); });
            }
        } catch (...) {
            stop_.store(true, std::memory_order_release);
            // 用假任務喚醒（也可使用專門的關閉旗標）
            for (unsigned i = 0; i < n; ++i) tasks_.enqueue(nullptr);
            for (auto& t : workers_) if (t.joinable()) t.join();
            throw;
        }
    }

    ~ThreadPoolMoody() { Stop(); }
    ThreadPoolMoody(const ThreadPoolMoody&) = delete;
    ThreadPoolMoody& operator=(const ThreadPoolMoody&) = delete;

    template<class F>
    void Submit(F&& f) {
        if (stop_.load(std::memory_order_acquire))
            throw std::runtime_error("ThreadPool stopped");
        // 直接 enqueue；BlockingConcurrentQueue 內部 lock-free + 等待者喚醒
        tasks_.enqueue(std::function<void()>(std::forward<F>(f)));
    }

    void Stop() {
        bool expected = false;
        if (!stop_.compare_exchange_strong(expected, true,
                                           std::memory_order_acq_rel)) return;
        // 放入與 worker 數量相同的「終止哨兵」(nullptr) 來喚醒所有等待者
        for (size_t i = 0; i < workers_.size(); ++i) tasks_.enqueue(nullptr);
        for (auto& t : workers_) if (t.joinable()) t.join();
        workers_.clear();
    }

private:
    void worker_loop() {
        for (;;) {
            std::function<void()> task;
            // 阻塞等待一筆任務（或哨兵）
            tasks_.wait_dequeue(task);
            if (!task) break; // 收到哨兵，退出
            try { task(); } catch (...) { /* 避免炸掉 worker */ }
        }
    }

    std::vector<std::thread> workers_;
    moodycamel::BlockingConcurrentQueue<std::function<void()>> tasks_;
    std::atomic<bool> stop_;
};

```

- 使用MPMCQueue

```cpp
// ThreadPoolRigtorp.h
#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

#include <rigtorp/MPMCQueue.h>

// 簡單計數信號量：只用來睡/喚醒，不影響 queue 的 lock-free
class counting_semaphore
{
public:
    explicit counting_semaphore(ptrdiff_t init = 0) : count_(init) {}
    void release(ptrdiff_t n = 1)
    {
        std::unique_lock<std::mutex> lk(m_);
        count_ += n;
        for (ptrdiff_t i = 0; i < n; ++i)
            cv_.notify_one();
    }
    void acquire()
    {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]
                 { return count_ > 0; });
        --count_;
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    ptrdiff_t count_;
};

class ThreadPoolRigtorp
{
public:
    explicit ThreadPoolRigtorp(unsigned n = std::thread::hardware_concurrency(),
                               size_t queue_capacity = 1 << 16)
        : stop_(false), q_(queue_capacity), items_(0)
    {
        if (n == 0)
            n = 1;
        workers_.reserve(n);
        try
        {
            for (unsigned i = 0; i < n; ++i)
            {
                workers_.emplace_back([this]
                                      { worker_loop(); });
            }
        }
        catch (...)
        {
            stop_.store(true, std::memory_order_release);
            // 喚醒所有等待者退出
            items_.release((ptrdiff_t)workers_.size());
            for (auto &t : workers_)
                if (t.joinable())
                    t.join();
            throw;
        }
    }

    ~ThreadPoolRigtorp() { Stop(); }
    ThreadPoolRigtorp(const ThreadPoolRigtorp &) = delete;
    ThreadPoolRigtorp &operator=(const ThreadPoolRigtorp &) = delete;

    template <class F>
    void Submit(F &&f)
    {
        if (stop_.load(std::memory_order_acquire))
            throw std::runtime_error("ThreadPool stopped");
        std::function<void()> task(std::forward<F>(f));
        // rigtorp::MPMCQueue 是有界；若滿，退讓重試（也可改為阻塞/丟棄策略）
        while (!q_.try_push(std::move(task)))
        {
            if (stop_.load(std::memory_order_acquire))
                throw std::runtime_error("ThreadPool stopped");
            std::this_thread::yield();
        }
        items_.release(1); // 新任務到，發一個 permit
    }

    void Stop()
    {
        bool expected = false;
        if (!stop_.compare_exchange_strong(expected, true,
                                           std::memory_order_acq_rel))
            return;
        // 喚醒所有 worker；他們看到 stop_ 後會清空殘餘任務並退出
        items_.release((ptrdiff_t)workers_.size());
        for (auto &t : workers_)
            if (t.joinable())
                t.join();
        workers_.clear();
    }

private:
    void worker_loop()
    {
        for (;;)
        {
            items_.acquire(); // 等待一個可取的項目或停止
            if (stop_.load(std::memory_order_acquire))
            {
                // 嘗試清空殘餘任務
                std::function<void()> task;
                while (q_.try_pop(task))
                {
                    safe_run(task);
                }
                return;
            }
            std::function<void()> task;
            if (q_.try_pop(task))
            {
                safe_run(task);
            }
        }
    }

    static void safe_run(std::function<void()> &task) noexcept
    {
        try
        {
            task();
        }
        catch (...)
        { /* logging 可加在此 */
        }
    }

    std::vector<std::thread> workers_;
    std::atomic<bool> stop_;
    rigtorp::MPMCQueue<std::function<void()>> q_;
    counting_semaphore items_;
};

```

- 主程序，要使用哪個套件，要自己做切換

```cpp
// main.cpp
#include <atomic>
#include <iostream>
#include <condition_variable>
#include "ThreadPoolMoody.h"
// #include "ThreadPoolRigtorp.h"

class CountDownLatch
{
public:
    explicit CountDownLatch(int count) : count_(count) {}
    void count_down()
    {
        std::unique_lock<std::mutex> lk(m_);
        if (--count_ == 0)
            cv_.notify_all();
    }
    void wait()
    {
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [&]
                 { return count_ == 0; });
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    int count_;
};

std::string FormatTimeNow(const char *format)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);

    char buf[20];
    std::strftime(buf, sizeof(buf), format, now_tm);
    return std::string(buf);
}

std::mutex io_mtx;

void Task()
{
    std::thread::id thread_id = std::this_thread::get_id();
    std::string now = FormatTimeNow("%H:%M:%S");
    {
        std::lock_guard<std::mutex> lock(io_mtx);
        std::cout << "Thread " << thread_id << ": wait succeeded"
                  << " (" << now << ")" << std::endl;
    }
}

int main()
{
    std::cout << "thread number: " << std::thread::hardware_concurrency() << std::endl;
    ThreadPoolMoody pool;            // moodycamel
    // ThreadPoolRigtorp pool; // rigtorp
    int tasks = 100000;
    CountDownLatch latch((int)tasks);
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < tasks; ++i)
    {
        pool.Submit([&latch]
                    { Task(); latch.count_down(); });
    }
    latch.wait();
    auto end = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double tps = (ms == 0) ? 0.0 : (1000.0 * tasks / ms);

    std::cout << "tasks=" << tasks
              << " time=" << ms << " ms"
              << " throughput=" << (uint64_t)tps << " tasks/s\n";
}

```

- 調整過後的CMakeLists.txt

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(tp_lockfree CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(concurrentqueue CONFIG REQUIRED)
find_package(MPMCQueue CONFIG REQUIRED)

add_executable(demo main.cpp ThreadPoolMoody.h ThreadPoolRigtorp.h)
target_link_libraries(demo PRIVATE concurrentqueue::concurrentqueue)
target_link_libraries(demo PRIVATE MPMCQueue::MPMCQueue)

if (UNIX AND NOT APPLE)
  target_link_libraries(demo PRIVATE pthread)
endif()

```

## 測試結果

這邊又重跑前面的結果，發現每次跑都慢了一點，但結果還是用了lock free是有比較快的。

```sh
# concurrentqueue
tasks=100000 time=6111 ms throughput=16363 tasks/s

# MPMCQueue
tasks=100000 time=6116 ms throughput=16350 tasks/s

# AI MPMCQueue
tasks=100000 time=6180 ms throughput=16181 tasks/s

# mutex queue
tasks=100000 time=6601 ms throughput=15149 tasks/s
```

