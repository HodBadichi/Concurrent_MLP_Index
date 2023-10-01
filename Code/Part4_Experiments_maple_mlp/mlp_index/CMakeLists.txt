include_directories("${PART3_DIR}/src/MlpRangeConcurrent" "${PART3_DIR}/test/LightRangeGenerator")
file(GLOB_RECURSE WORKLOAD_FILES "${PART3_DIR}/test/Workloads/*")

add_executable(MlpRangeBenchMark test_mlp.cpp ../util.cpp ${PART3_DIR}/test/LightRangeGenerator/LightRangeGenerator.h ${PART3_DIR}/test/LightRangeGenerator/LightRangeGenerator.cpp ${PART3_DIR}/test/RangeSet/RangeSet.cpp ${WORKLOAD_FILES})

target_link_libraries(MlpRangeBenchMark PUBLIC Range_concurrent_lib gtest)
target_include_directories(MlpRangeBenchMark PRIVATE ${RANGE_CONCURRENT_DIR})

