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