#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#define LOCK_GUARD 1
#define TRY_LOCK 0

using namespace std;

mutex mtx;
recursive_mutex rec_mtx;
int g_sum = 0;

void IncrementSomething()
{
#if LOCK_GUARD
    lock_guard<mutex> lock(mtx);
#endif

#if TRY_LOCK
    while (!mtx.try_lock())
    {
        this_thread::sleep_for(chrono::microseconds(100));
    }
#endif

    for (size_t i = 0; i < 1000000; i++)
    {
        g_sum ++;
    }
    
    cout << "sum is " << g_sum << endl;
#if TRY_LOCK
    mtx.unlock();    
#endif
}

void Func2()
{
    lock_guard<recursive_mutex> lock(rec_mtx);
    cout << "hihi\n";
}

void Func1()
{
    lock_guard<recursive_mutex> lock(rec_mtx);
    Func2();
}

timed_mutex time_mtx;
void work()
{
	chrono::milliseconds timeout(100);

	if (time_mtx.try_lock_for(timeout))
    {
        cout << this_thread::get_id() << ": do work with the mutex" << endl;
        chrono::milliseconds sleepDuration(250);
        this_thread::sleep_for(sleepDuration);
        time_mtx.unlock();
    }
    else
    {
        cout << this_thread::get_id() << ": do work without the mutex" << endl;
    }
}

int main(int argc, char const *argv[])
{
    // const int thrade_num = 15;
    // thread threads[thrade_num];
    // for (size_t i = 0; i < thrade_num; i++)
    // {
    //     threads[i] = thread(IncrementSomething);
    // }
    
    // chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    // for (size_t i = 0; i < thrade_num; i++)
    // {
    //     threads[i].join();
    // }
    // chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    // cout << "Printing " << (TRY_LOCK ? "TRY_LOCK" : "LOCK_GUARD") << " took " 
    //     << chrono::duration_cast<chrono::microseconds>(t2 - t1).count()
    //     << "us.\n";

    // thread t1(IncrementSomething);
    // thread t2(IncrementSomething);

    // t1.join();
    // t2.join();

    // Func1();

    thread t1(work);
	thread t2(work);
	t1.join();
	t2.join();

    return 0;
}

