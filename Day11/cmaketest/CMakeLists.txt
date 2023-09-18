cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

set(ServerSRC
    server.cpp
)

find_package(websocketpp CONFIG REQUIRED)

add_executable(server ${ServerSRC})

set(LIB
    pthread
)

target_link_libraries(server PRIVATE websocketpp::websocketpp ${LIB})