cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++14 -fpermissive") 

set(SRC_producer
    producer.cpp
)

set(SRC_consumer
    consumer.cpp
)

find_package(RdKafka CONFIG REQUIRED)

add_executable(producer ${SRC_producer})
add_executable(consumer ${SRC_consumer})

target_link_libraries(producer PRIVATE RdKafka::rdkafka RdKafka::rdkafka++)
target_link_libraries(consumer PRIVATE RdKafka::rdkafka RdKafka::rdkafka++)