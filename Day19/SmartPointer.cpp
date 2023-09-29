#include <iostream>
#include <string>
#include <memory>

using namespace std;

class Cat
{
public:
    Cat() {}
    Cat(string name, int tail):Name(name), TailLen(tail) {}
    ~Cat()
    {
        cout << "Cat " << Name << " free\n";
    }
    string Name = "";
    int TailLen = 0;
};

int main(int argc, char const *argv[])
{
    // 建議用make_shared替代new，因為分配記憶體的位址會有優化，記憶體分配較連續，對性能好
    shared_ptr<Cat> pCat = make_shared<Cat>("cat1", 49);
    cout << "pCat count: " << pCat.use_count() << endl;
    {
        shared_ptr<Cat> pCat2 = pCat;
        cout << "pCat count: " << pCat.use_count() << endl;
        // Cat2跟Cat是同一個物件，離開時引數為1，還不會自動釋放
    }
    cout << "pCat count: " << pCat.use_count() << endl;
    {
        shared_ptr<Cat> pCat3 = make_shared<Cat>("cat3", 55);
        // 會比cat1先釋放，離開作用域，引數就為0
    }

    // make_unique是C++14才有的語法
    unique_ptr<Cat> pCat4 = make_unique<Cat>("cat4", 16);
    // 下面會編譯錯誤
    // unique_ptr<Cat> pCat5 = pCat4;
    // 但可以透過move去轉移擁有權，move下次再介紹
    unique_ptr<Cat> pCat6 = move(pCat4); // cat4就不能用了

    // move也能將unique_ptr轉成shared_ptr，但不能將shared_ptr轉成unique_ptr
    shared_ptr<Cat> pCat7 = move(pCat6);

    weak_ptr<Cat> wp(pCat7);
    cout << "pCat7 count: " << wp.use_count() << endl;
    if (wp.expired())
    {
        cout << "wp was expired\n";
    }
    else
    {
        cout << "wp was not expired\n";
    }
    
    // 手動釋放
    pCat7.reset();

    if (wp.expired())
    {
        cout << "wp was expired\n";
    }
    else
    {
        cout << "wp was not expired\n";
    }

    return 0;
}
