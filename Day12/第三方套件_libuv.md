# 第三方套件 libuv

[libuv](https://github.com/libuv/libuv)是一個跨平台異步事件驅動的IO函式庫。

## 環境

一樣是用之前的`Dockerfile`

```shell
docker run -v D:/dvt:/home --name=cpp_dev -itd cpp_dev
```

## 安裝

安裝也是老樣子，`vcpkg`

```shell
vcpkg install libuv
# 安裝完成信息
libuv provides CMake targets:

    find_package(libuv CONFIG REQUIRED)
    target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
```

## 使用

使用範例就用官方的[cat指令範例](https://github.com/libuv/libuv/blob/v1.x/docs/code/uvcat/main.c)，針對一個檔案將內容輸出到標準輸出上。

`CMakeLists.txt`跟`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake`

```cmake
cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

set(SRC
    smain.cpp
)

find_package(libuv CONFIG REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
```

最後就是實測階段

```shell
cd build
make
echo "hahaha" > test.txt
./cmaketest test.txt
# output
hahaha
```

`libuv`的內容還是很多的，之後有機會再補全。

## 參考

[libuv-book](https://luohaha.github.io/Chinese-uvbook/source/basics_of_libuv.html)
