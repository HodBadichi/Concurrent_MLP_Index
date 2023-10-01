set(MAPLETREE_DIR "${PROJECT_SOURCE_DIR}/Code/Part2_range_tree/src/maple_tree_rcu_support")
file(GLOB_RECURSE WORKLOAD_FILES "${PART3_DIR}/test/Workloads/*")
include_directories("${PART3_DIR}/src/MlpRangeConcurrent" "${PART3_DIR}/test/LightRangeGenerator")

include_directories(MAPLETREE_DIR)

add_executable(MapleTreeBenchMark test_maple_tree.cpp ../util.cpp ${PART3_DIR}/test/LightRangeGenerator/LightRangeGenerator.h ${PART3_DIR}/test/LightRangeGenerator/LightRangeGenerator.cpp ${PART3_DIR}/test/RangeSet/RangeSet.cpp ${WORKLOAD_FILES})



target_link_libraries(MapleTreeBenchMark PUBLIC MapleTree_lib gtest)
target_include_directories(MapleTreeBenchMark PRIVATE ${MAPLETREE_DIR})

