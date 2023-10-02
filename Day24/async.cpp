#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std;

int CalcNum ()
{
    cout << "calculatng!" << endl;
    return 20;
}

int add(int a, int b)
{
    cout << "add work!\n";
    return a + b;
}

int main(int argc, char const *argv[])
{
    // // future會是async的回傳，launch::async非同步策略會馬上執行
    // auto future = async(launch::async, CalcNum);

    // cout << "hello async " << endl;
    // this_thread::sleep_for(chrono::seconds(1));
    // cout << "answer is " << future.get() << endl;

    // // launch::deferred則是延遲到future變數執行wait或get才會執行
    // auto future2 = async(launch::deferred, CalcNum);

    // cout << "hello deferred " << endl;
    // this_thread::sleep_for(chrono::seconds(1));
    // cout << "answer is " << future2.get() << endl;
    
    // promise<int> pAnswer;

    // // 傳遞數據的子線程
    // auto t1 = thread([&pAnswer]
    // {
    //     cout << "thread begin\n";
    //     this_thread::sleep_for(chrono::seconds(1));
    //     pAnswer.set_value(100);
    //     cout << "thread end\n";
    // });

    // // 主線程等數據
    // cout << pAnswer.get_future().get() << endl;
    // t1.join();


    // auto promise = std::promise<int>();
    // auto shared_fu = shared_future<int>(promise.get_future());

    // auto t2 = thread([&promise]
    // {
    //     cout << "thread 2 begin\n";
    //     this_thread::sleep_for(chrono::seconds(2));
    //     promise.set_value(100);
    //     cout << "thread 2 end\n";
    // });

    // auto t3 = thread([shared_fu]
    // {
    //     cout << "thread 3 begin\n";
    //     // 若promise尚未set_value就會阻塞
    //     auto value = shared_fu.get();
    //     cout << "thread 3 get value " << value << endl;
    //     cout << "thread 3 end\n";
    // });

    // auto value = shared_fu.get();
    // cout << "main get value " << value << endl;

    // t2.join();
    // t3.join();

    packaged_task<int(int, int)> task(add);
    future<int> result = task.get_future();

    thread t4(move(task), 10, 20);
    cout << "result is " << result.get() << endl;

    t4.join();

    return 0;
}
