add_executable(MlpSetInsertRange RangeTests.cpp)


set(RANGE_DIR "${PROJECT_SOURCE_DIR}/Code/Part2_range_tree/src/MlpRangeSupport")

add_test(NAME MlpSetInsertRange COMMAND MlpSetInsertRange)


target_link_libraries(MlpSetInsertRange PUBLIC Range_lib gtest)
target_include_directories(MlpSetInsertRange PRIVATE ${RANGE_DIR})


