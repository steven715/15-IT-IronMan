#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

mutex mtx;
condition_variable cv;
bool ready = false;

void Work_thread()
{
    while (true)
    {
        unique_lock<mutex> lk(mtx);
        // cv.wait(lk);
        cv.wait(lk, []{return ready;});
        cout << this_thread::get_id() << " awake!\n";
        ready = false;
    }    
}

void Prepare_thread()
{
    while (true)
    {
        {
            lock_guard<mutex> lk(mtx);
            ready = true;
            cout << this_thread::get_id() << " ready to notify!\n";
        }
        cv.notify_one();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }    
}

void LatchWork()
{
    
}

int main(int argc, char const *argv[])
{
    thread t1(Work_thread);
    thread t2(Prepare_thread);

    t1.join();
    t2.join();

    return 0;
}
