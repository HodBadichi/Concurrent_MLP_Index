file(GLOB_RECURSE WORKLOAD_FILES "Workloads/*")

add_executable(OriginMlp benchmarks.cpp ${WORKLOAD_FILES} StupidTrie/Stupid64bitIntegerTrie.cpp StupidTrie/Stupid64bitIntegerTrie.h
)

add_executable(ConcurrentMlpSet benchmarks.cpp ${WORKLOAD_FILES} StupidTrie/Stupid64bitIntegerTrie.cpp StupidTrie/Stupid64bitIntegerTrie.h)

set(ORIGIN_DIR "${PROJECT_SOURCE_DIR}/Code/Part1_concurrent_mlp_index/src/Origin")
set(CONCURRENT_DIR "${PROJECT_SOURCE_DIR}/Code/Part1_concurrent_mlp_index/src/Versioning")

add_test(NAME OriginMlp COMMAND OriginMlp)
add_test(NAME ConcurrentMlpSet COMMAND ConcurrentMlpSet)


target_link_libraries(OriginMlp PUBLIC ORIGIN_LIB gtest)
target_include_directories(OriginMlp PRIVATE ${ORIGIN_DIR})

target_link_libraries(ConcurrentMlpSet PUBLIC CONCURRENT_LIB gtest)
target_include_directories(ConcurrentMlpSet PRIVATE ${CONCURRENT_DIR})
