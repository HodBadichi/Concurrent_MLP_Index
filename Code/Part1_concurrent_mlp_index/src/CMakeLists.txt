set(ORIGIN_DIR "Origin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mavx2 -g -DNDEBUG")

add_library(ORIGIN_LIB STATIC ${ORIGIN_DIR}/common.h ${ORIGIN_DIR}/fasttime.h ${ORIGIN_DIR}/MlpSetUInt64.h ${ORIGIN_DIR}/MlpSetUInt64.cpp)

target_include_directories(ORIGIN_LIB PRIVATE ${ORIGIN_DIR})


set(CONCURRENT_DIR "Versioning")

add_library(CONCURRENT_LIB STATIC ${CONCURRENT_DIR}/common.h ${CONCURRENT_DIR}/fasttime.h ${CONCURRENT_DIR}/MlpSetUInt64.h ${CONCURRENT_DIR}/MlpSetUInt64.cpp)

target_include_directories(CONCURRENT_LIB PRIVATE ${CONCURRENT_DIR})