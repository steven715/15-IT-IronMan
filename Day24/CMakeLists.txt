cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(cmaketest)                  # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++17") 

set(SRC
    smain.cpp
)

set(SP_SRC
    SmartPointer.cpp
)

set(WP_SRC
    WildPointer.cpp
)

set(LIB
    pthread
)

add_executable(${PROJECT_NAME} ${SRC})
add_executable(smart_pointer ${SP_SRC})
add_executable(wild_pointer ${WP_SRC})
add_executable(test_mutex TestMutex.cpp)
add_executable(rw_lock RWLock.cpp)
add_executable(async async.cpp)

target_link_libraries(test_mutex ${LIB})
target_link_libraries(rw_lock ${LIB})
target_link_libraries(async ${LIB})