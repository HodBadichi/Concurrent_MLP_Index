cmake_minimum_required(VERSION 3.14)
project(concurrent_mlpds)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mavx2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread")  # Add -pthread for debugging
include_directories("${PROJECT_SOURCE_DIR}/Code/Utils/userspace-rcu-lib/include")
link_directories("${PROJECT_SOURCE_DIR}/Code/Utils/userspace-rcu-lib/lib")


set(PART1_DIR "${PROJECT_SOURCE_DIR}/Code/Part1_concurrent_mlp_index")
set(PART2_DIR "${PROJECT_SOURCE_DIR}/Code/Part2_range_tree")
set(PART3_DIR "${PROJECT_SOURCE_DIR}/Code/Part3_concurrent_ranged_mlp")
set(PART4_DIR "${PROJECT_SOURCE_DIR}/Code/Part4_Experiments_maple_mlp")

add_subdirectory(Code/Utils/)
add_subdirectory(Code/Part1_concurrent_mlp_index)
add_subdirectory(Code/Part2_range_tree)
add_subdirectory(Code/Part3_concurrent_ranged_mlp)
add_subdirectory(Code/Part4_Experiments_maple_mlp)


