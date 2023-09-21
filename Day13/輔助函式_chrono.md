# 輔助函式 chrono

今天來介紹C++11出來的時間函式庫`chrono`，最初是源自`boost`的某個專案中，以及字串操作的`split`，因為這個在C++中都沒有源生的支援，以及`replace`。

## 環境

環境不免俗的還是使用之前的`Dockerfile`

```shell
docker run -v D:/dvt:/home --name=cpp_dev -itd cpp_dev
```

## chrono

`chrono`主要功能是拿來做紀錄時間長度，參考以下代碼

```cpp
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
    // 取得當下時間
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::cout << "Hello World\n";
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    // 取得經過時間，duration能支援nanoseconds(奈秒)、microseconds(微秒)、milliseconds(毫秒)、seconds(秒)、minutes(分)、hours(小時)
    std::cout << "Printing took "
        << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
        << "us.\n";
    
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    // time_point也可以轉成時間戳
    std::time_t now_c = std::chrono::system_clock::to_time_t( now );
    // 然後透過ctime()輸出成時間表示字串
    std::cout << std::ctime( &now_c ) << std::endl;

    return 0;
}
```

## split

其實C語言有提供類似字串分割的功能`strtok`，但原字串也會變動，所以可以利用`string`中的`find`跟`substr`來做到字串切割的功能

```cpp
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void split(const string& s, vector<string>& tokens, const string& delimiters = " ")
{
    string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

int main ()
{
    char str100[] = "Hello world, nice to meet you";
    const char* d = "  ,";
    char *p;
    p = strtok(str100, d);
    
    while (p != NULL) {
        cout << p << endl;
        p = strtok(NULL, d);  
    }

    string text = "Hello world, nice to meet you.";
    vector<string> results;
    split(text, results);
    for (auto &s : results)
    {
        cout << s << endl;
    }    

    return 0;
}
```

## replace

使用代碼參考下方

```cpp
#include <iostream>
#include <string>

using namespace std;

int main ()
{
    string str = "this is a test string.";
    // 指定位置
    str.replace(0, 4, "that");
    cout << str << endl;

    // 指定iterator
    str.replace(str.begin(), str.begin()+4, "this");
    cout << str << endl;
  return 0;
}
```

## 參考

[C++11 STL 的時間函式庫：chrono](https://kheresy.wordpress.com/2013/12/27/c-stl-chrono/)
[C/C++ 字串分割的3種方法](https://shengyu7697.github.io/cpp-string-split/)
[cpp string replace](https://cplusplus.com/reference/string/string/replace/)
