cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(atomic_practice) # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++17")

find_package(Threads REQUIRED)

set(thread_pool thread_pool)
add_executable(${thread_pool} thread_pool.cpp)
target_link_libraries(${thread_pool} PRIVATE Threads::Threads)
