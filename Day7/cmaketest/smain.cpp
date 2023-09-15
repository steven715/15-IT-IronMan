#include <iostream>
#include <uuid/uuid.h>

int main()
{
    int i, n;
    uuid_t uu[4];
    char buf[1024];
    struct timeval tv;
    // 1、
    uuid_generate(uu[0]);
    // 2、
    uuid_generate_random(uu[1]);
    // 3、
    uuid_generate_time(uu[2]);
    // 4、
    uuid_generate_time_safe(uu[3]);
    for (i = 0; i < 4; ++i)
    {
        uuid_unparse(uu[i], buf);
        std::cout << "uu " << i << " " << buf << std::endl;
    }

    return 0;
}
