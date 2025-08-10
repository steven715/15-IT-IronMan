#include <thread>
#include <atomic>
#include <assert.h>

using namespace std;

atomic<bool> x, y;
atomic<int> z;

void write_x_then_y()
{
    x.store(true, memory_order_relaxed);
    y.store(true, memory_order_relaxed);    
}

void read_y_then_x()
{
    while (!y.load(memory_order_relaxed))
        ;
    if (x.load(memory_order_relaxed))
        ++z;
}

int main(int argc, char const *argv[])
{
    x = false;
    y = false;
    z = 0;
    thread a(write_x_then_y);
    thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() != 0);

    return 0;
}