#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

struct Cat
{
    Cat(string name, int tail) : Name(name), TailLength(tail) {}
    string Name;
    int TailLength;

    // override 運算子 < 也行
    bool operator < (const Cat& c) const {
        return TailLength < c.TailLength;
    }
};

class CompareCat
{
    public:
    bool operator() (const Cat& c1, const Cat& c2) {
        return c1.TailLength > c2.TailLength;
    }
};

int main(int argc, char const *argv[])
{
    // map 排序只能針對key
    map<Cat, int, CompareCat> catOrderByTailDesc;
    catOrderByTailDesc.insert(make_pair(Cat("meme", 2), 2));
    catOrderByTailDesc.insert(make_pair(Cat("kitty", 35), 6));
    catOrderByTailDesc.insert(make_pair(Cat("zupi", 9), 5));
    
    for (auto &cat : catOrderByTailDesc)
    {
        cout << cat.first.Name << " " << cat.first.TailLength << endl;
    }

    // set 也一樣
    set<Cat, CompareCat> catOrderByTailDescSet;
    catOrderByTailDescSet.insert(Cat("meme", 2));
    catOrderByTailDescSet.insert(Cat("kitty", 35));
    catOrderByTailDescSet.insert(Cat("zupi", 9));
    
    for (auto &cat : catOrderByTailDescSet)
    {
        cout << cat.Name << " " << cat.TailLength << endl;
    }

    // override
    set<Cat> catOverride;
    catOverride.insert(Cat("meme", 2));
    catOverride.insert(Cat("kitty", 35));
    catOverride.insert(Cat("zupi", 9));
    for (auto &cat : catOrderByTailDescSet)
    {
        cout << cat.Name << " " << cat.TailLength << endl;
    }

    return 0;
}
