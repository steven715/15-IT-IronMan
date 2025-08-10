cmake_minimum_required(VERSION 3.10) # 設定最低版本要求
project(atomic_practice) # 專案名稱

set(CMAKE_CXX_FLAGS "-std=c++17")

find_package(Threads REQUIRED)

set(seq_cst seq_cst)
add_executable(${seq_cst} memory_order_seq_cst.cpp)
target_link_libraries(${seq_cst} PRIVATE Threads::Threads)

set(relaxed relaxed)
add_executable(${relaxed} memory_order_relaxed.cpp)
target_link_libraries(${relaxed} PRIVATE Threads::Threads)

set(acquire_release acquire_release)
add_executable(${acquire_release} memory_order_acquire_release.cpp)
target_link_libraries(${acquire_release} PRIVATE Threads::Threads)

set(relaxed_with_acq_rel relaxed_with_acq_rel)
add_executable(${relaxed_with_acq_rel} memory_order_relaxed_with_acq_rel.cpp)
target_link_libraries(${relaxed_with_acq_rel} PRIVATE Threads::Threads)

set(relaxed_with_fence relaxed_with_fence)
add_executable(${relaxed_with_fence} memory_order_relaxed_with_fence.cpp)
target_link_libraries(${relaxed_with_fence} PRIVATE Threads::Threads)
