#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>

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

class ThreadPoolBetter {
public:
    explicit ThreadPoolBetter(unsigned n = std::thread::hardware_concurrency()) : stop_(false) {
        if (n == 0) n = 1;
        workers_.reserve(n);
        try {
            for (unsigned i = 0; i < n; ++i) {
                workers_.emplace_back([this]{ this->worker_loop(); });
            }
        } catch (...) {
            stop_ = true;
            cv_.notify_all();
            for (auto& t : workers_) if (t.joinable()) t.join();
            throw;
        }
    }
    ~ThreadPoolBetter() {
        Stop();
    }
    ThreadPoolBetter(const ThreadPoolBetter&) = delete;
    ThreadPoolBetter& operator=(const ThreadPoolBetter&) = delete;

    template<class F>
    void Submit(F&& f) {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            if (stop_) throw std::runtime_error("stopped");
            tasks_.emplace(std::forward<F>(f));
        }
        cv_.notify_one();
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            if (stop_) return;
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& t : workers_) if (t.joinable()) t.join();
        workers_.clear();
    }

private:
    void worker_loop() {
        for (;;) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lk(mtx_);
                cv_.wait(lk, [this]{ return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            try { task(); } catch (...) { /* swallow for bench */ }
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> stop_;
};

class ThreadPool
{
public:
    ThreadPool() : done(false)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try
        {
            for (unsigned i = 0; i < thread_count; i++)
            {
                threads_.push_back(std::thread(&ThreadPool::work_thread, this));
            }
        }
        catch (const std::exception &e)
        {
            done = true;
        }
    }

    ~ThreadPool()
    {
        done = true;
        for (auto& t : threads_) if (t.joinable()) t.join();
    }

    template <typename Func>
    void Submit(Func f)
    {
        std::lock_guard<std::mutex> lk(mtx);
        task_queue_.push(std::function<void()>(f));
    }

private:
    std::mutex mtx;
    std::queue<std::function<void()>> task_queue_;
    std::vector<std::thread> threads_;
    std::atomic_bool done;

    void work_thread()
    {
        while (!done)
        {
            std::function<void()> task;
            std::lock_guard<std::mutex> lk(mtx);
            if (!task_queue_.empty())
            {
                task = task_queue_.front();
                task();
                task_queue_.pop();
            }
        }
    }
};

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
    ThreadPool thread_pool;
    // ThreadPoolBetter thread_pool;
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
