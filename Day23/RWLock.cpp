#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>

using namespace std;

mutex mtx;

std::string FormatTimeNow(const char* format) {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm* now_tm = std::localtime(&now_c);

  char buf[20];
  std::strftime(buf, sizeof(buf), format, now_tm);
  return std::string(buf);
}

class RWLockNum
{
private:
    int num = 0;
    mutable shared_mutex shared_mtx;
public:
    int Increment()
    {
        lock_guard<shared_mutex> lk(shared_mtx);
        return ++num;
    }

    int GetNum()
    {
        shared_lock<shared_mutex> lk(shared_mtx);
        cout << FormatTimeNow("%H:%M:%S");
        return num;
    }
};

RWLockNum counter;

void WriteFunc()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(300));
        unique_lock<mutex> lock(mtx);
        cout << this_thread::get_id() << " Write g_num: " << counter.Increment() << endl;
    }
}

void ReadFunc()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
        unique_lock<mutex> lock(mtx);
        cout << this_thread::get_id() << " Read g_num: " << counter.GetNum() << endl;
    }
}

int main(int argc, char const *argv[])
{
    thread t1(WriteFunc);
    thread t2(ReadFunc);
    thread t3(ReadFunc);
    thread t4(ReadFunc);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
