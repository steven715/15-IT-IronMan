# 第三方套件 libuuid curl

今天來介紹兩個常用的第三方套件。

## 環境

環境一樣，前一天的`Dockerfile`，然後專案拿Day1的`cmaketest`資料夾過來用。

```shell
docker run -v D:/dvt:/home --name=cpp_dev --tty cpp_dev
```

## libuuid

`uuid`是通用唯一識別碼(Universally Unique Identifer)，主要是由機器提供一串數字，保證其唯一性。

### 安裝 libuuid

安裝一樣`vcpkg`。

```shell
vcpkg install libuuid
# 安裝信息
libuuid provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(unofficial-libuuid CONFIG REQUIRED)
    target_link_libraries(main PRIVATE unofficial::UUID::uuid)
```

### 使用 libuuid

代碼請參考下方

```cpp
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
```

然後是`CMakeLists.txt`，再下指令`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake`

```cmake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

set(SRC
    smain.cpp
)

find_package(unofficial-libuuid CONFIG REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::UUID::uuid)
```

最後就是來看一下`uuid`的樣子

```shell
cd build
make
./cmaketest
# output
uu 0 259eb025-a722-46cf-8e9a-7fdd05f32872
uu 1 f8c564ee-8508-465d-861b-d2224cbf601a
uu 2 f9193980-53d6-11ee-9acf-af9da9fe60ae
uu 3 f919398a-53d6-11ee-9acf-af9da9fe60ae
```

## curl

`curl`是一個支持多種通訊協議的客戶端函式庫，像網頁瀏覽的HTTP協議GET方法，或是檔案傳輸的FTP都支援。

### 安裝 curl

安裝也是透過`vcpkg`，但需要先安裝個工具，不然會安裝不瞭。

```shell
yum -y install perl-IPC-Cmd
vcpkg install curl
# output
curl provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(CURL CONFIG REQUIRED)
    target_link_libraries(main PRIVATE CURL::libcurl)
```

### 使用 curl

代碼就用最簡單網頁請求來做示範。

```cpp
#include <iostream>
#include <curl/curl.h>

int main (int argc, char * argv[])
{
    CURL *curl;
    CURLcode res;
    if (argc != 2)
    {
        std::cout << "Usage: file url \n";
        abort();
    }
    
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }    

    return 0;
}
```

然後是`CMakeLists.txt`跟下指令`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake`

```cmake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

set(SRC
    smain.cpp
)

set(SRC_curl
    curl.cpp
)

find_package(unofficial-libuuid CONFIG REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::UUID::uuid)

find_package(CURL CONFIG REQUIRED)
add_executable(curl_demo ${SRC_curl})
target_link_libraries(curl_demo PRIVATE CURL::libcurl)
```

最後就是看一下結果

```shell
cd build
make
./cmaketest
./curl_demo https://ithelp.ithome.com.tw/articles/10314368
# output 我Day1文章的內容
```

## 參考資料

[linux下安装使用libuuid(uuid-generate)](https://www.cnblogs.com/oloroso/p/4633744.html)
