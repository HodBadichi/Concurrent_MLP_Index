file(GLOB_RECURSE WORKLOAD_FILES "Workloads/*")

include_directories(RangeSet PerfectRangeGenerator LightRangeGenerator)

add_executable(MlpRangeConcurrent
        test_mlprangeconcurrent_index.cpp
        ${WORKLOAD_FILES}
        StupidTrie/Stupid64bitIntegerTrie.cpp
        StupidTrie/Stupid64bitIntegerTrie.h
        RangeSet/RangeSet.cpp
        RangeSet/RangeSet.h
        PerfectRangeGenerator/PerfectRangeGenerator.cpp
        PerfectRangeGenerator/PerfectRangeGenerator.h
        LightRangeGenerator/LightRangeGenerator.cpp
        LightRangeGenerator/LightRangeGenerator.h)

set(RANGE_CONCURRENT_DIR "${PROJECT_SOURCE_DIR}/Code/Part3_concurrent_ranged_mlp/src/MlpRangeConcurrent")

add_test(NAME MlpRangeConcurrent COMMAND MlpRangeConcurrent)


target_link_libraries(MlpRangeConcurrent PUBLIC Range_concurrent_lib gtest)
target_include_directories(MlpRangeConcurrent PRIVATE ${RANGE_CONCURRENT_DIR})

