cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14") 

include_directories(${PROJECT_SOURCE_DIR})

set(SRC
    smain.cpp
)

find_package(flatbuffers CONFIG REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE flatbuffers::flatbuffers)