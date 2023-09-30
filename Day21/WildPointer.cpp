#include <iostream>
#include <memory>
#include <string>

using namespace std;

class Cat
{
private:
    string Name = "";
    int Tail = 0;
public:
    Cat(/* args */);
    ~Cat();

    void Meow()
    {
        cout << Name << " Meow!\n";
    }
};

Cat::Cat(/* args */)
{
}

Cat::~Cat()
{
    cout << "cat " << Name << " delete\n";
}


int main(int argc, char const *argv[])
{
    char* str = (char *)malloc (1024);
    return 0;
}
