#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <iostream>

std::string FormatTimeNow(const char *format)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);

    char buf[20];
    std::strftime(buf, sizeof(buf), format, now_tm);
    return std::string(buf);
}

class Semaphore
{
public:
    explicit Semaphore(int count = 0) : count_(count) {}

    void Signal()
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.notify_one();
        count_++;
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [=]
                { return count_ > 0; });
        count_--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count_;
};

Semaphore sem(4);
std::mutex io_mtx;

void Task()
{
    sem.Wait();

    std::thread::id thread_id = std::this_thread::get_id();
    std::string now = FormatTimeNow("%H:%M:%S");
    {
        std::lock_guard<std::mutex> lock(io_mtx);
        std::cout << "Thread " << thread_id << ": wait succeeded"
                  << " (" << now << ")" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    sem.Signal();
}

int main(int argc, char const *argv[])
{
    int threadNum = 4;
    std::vector<std::thread> v;
    v.reserve(threadNum);
    for (std::size_t i = 0; i < threadNum; ++i)
    {
        v.emplace_back(&Task);
    }
    for (std::thread &t : v)
    {
        t.join();
    }
    return 0;
}
