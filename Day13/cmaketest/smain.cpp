#include <iostream>
#include <chrono>
#include <string>
#include <string.h> // strtok
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

    string str = "this is a test string.";
    // 指定位置
    str.replace(0, 4, "that");
    cout << str << endl;

    // 指定iterator
    str.replace(str.begin(), str.begin()+4, "this");
    cout << str << endl;

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
