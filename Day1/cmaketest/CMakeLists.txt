cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-7/root/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-std=c++14") 

set(SRC
    smain.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
