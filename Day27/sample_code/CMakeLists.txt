cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(atomic_practice) # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++17")

find_package(Threads REQUIRED)

set(semaphore semaphore)
add_executable(${semaphore} semaphore.cpp)
target_link_libraries(${semaphore} PRIVATE Threads::Threads)
