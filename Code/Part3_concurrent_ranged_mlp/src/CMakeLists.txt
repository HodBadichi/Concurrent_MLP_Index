set(RANGE_CONCURRENT_DIR "MlpRangeConcurrent")

add_library(Range_concurrent_lib STATIC ${RANGE_CONCURRENT_DIR}/common.h ${RANGE_CONCURRENT_DIR}/fasttime.h ${RANGE_CONCURRENT_DIR}/MlpSetUInt64.h ${RANGE_CONCURRENT_DIR}/RangeModule.h ${RANGE_CONCURRENT_DIR}/MlpSetUInt64.cpp ${RANGE_CONCURRENT_DIR}/RangeModule.cpp)

target_include_directories(Range_concurrent_lib PRIVATE ${RANGE_CONCURRENT_DIR})