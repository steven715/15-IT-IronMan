# 第三方套件 jsoncpp, ajson

今天來介紹C++中用於解析跟序列化Json格式資料的兩個套件。

## 環境

環境就延續Day4的`Dockerfile`，重啟一個新的Container來做Demo。

```shell
docker run -v D:/dvt:/home --name=cpp_dev --tty cpp_dev
```

## jsoncpp

首先介紹[jsoncpp](https://github.com/open-source-parsers/jsoncpp/tree/master)，是一個能夠解析Json文件的函式庫，甚至能解析文件中的註解。

### 安裝 jsoncpp

安裝的部分，一樣是靠`vcpkg`來搞定。

```shell
vcpkg install jsoncpp
```

成功的話，應該會看到以下信息。

```shell
jsoncpp provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(jsoncpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE JsonCpp::JsonCpp)
```

然後再透過`CMake`的`CLI`設定專案，如果不想用`CLI`，也可以參考Day2中提到，在VS Code的`settings.json`加入相對應的欄位設定。

```shell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 使用 jsoncpp

這邊就拿官方的example展示。

```cpp
#include "json/json.h"
#include <fstream>
#include <iostream>
 
int main(int argc, char* argv[]) {
  Json::Value root;
  std::ifstream ifs;
  ifs.open(argv[1]);

  Json::CharReaderBuilder builder;
  builder["collectComments"] = true;
  JSONCPP_STRING errs;
  if (!parseFromStream(builder, ifs, &root, &errs)) {
    std::cout << errs << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << root << std::endl;
  return EXIT_SUCCESS;
}
```

json file:

```json
// comment head
{
    // comment before
    "key" : "value"
    // comment after
}// comment tail
```

```shell
cd build/
make
./jsoncpp_demo ../demo.json
# output
// comment head
{
        // comment before
        "key" : "value"
} // comment tail
// comment after
```

## ajson

[ajson](https://github.com/lordoffox/ajson)也是一個能夠解析json格式的函式庫，但他更特別的地方在於他能將json物件轉成C++中的結構體(struct)。

### 安裝 ajson

ajson安裝的話，就不能透過`vcpkg`了，因為vcpkg中目前並沒有包含ajson，但ajson其實只需要有頭文件就可以使用，所以我們只需要從`github`抓下來即可使用。

```shell
git clone https://github.com/lordoffox/ajson.git
```

## 使用 ajson

這邊也是一樣使用官方的example。

```cpp
#include <iostream>
#include <string>

#include "ajson.hpp"

using namespace std;
using namespace ajson;

struct demo
{
  string hello;
  string world;
};

AJSON(demo,hello,world)

int main(int argc, char * argv[])
{
 char * buff = "{\"hello\" : \"Hello\", \"world\" : \"world.\"}";
 demo the_demo;
 load_from_buff(the_demo,buff);
 cout << the_demo.hello << " " << the_demo.world << std::endl;
 cin.get();
 return 0;
}
```

```shell
cd build/
make
./ajson_demo
# output
Hello world.
```
