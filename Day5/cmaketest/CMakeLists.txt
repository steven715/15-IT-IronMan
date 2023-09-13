cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

include_directories(./ajson)

set(SRC
    smain.cpp
)

set(SRC_ajson
ajson_demo.cpp
)

find_package(jsoncpp CONFIG REQUIRED)

add_executable(jsoncpp_demo ${SRC})

target_link_libraries(jsoncpp_demo PRIVATE JsonCpp::JsonCpp)

add_executable(ajson_demo ${SRC_ajson})
