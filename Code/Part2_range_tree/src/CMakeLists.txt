

set(RANGE_DIR "${PROJECT_SOURCE_DIR}/Code/Part2_range_tree/src/MlpRangeSupport")
add_library(Range_lib STATIC ${RANGE_DIR}/common.h ${RANGE_DIR}/fasttime.h ${RANGE_DIR}/MlpSetUInt64.h ${RANGE_DIR}/RangeModule.h ${RANGE_DIR}/MlpSetUInt64.cpp ${RANGE_DIR}/RangeModule.cpp)
target_include_directories(Range_lib PRIVATE ${RANGE_DIR})

set(MAPLETREE_DIR "${PROJECT_SOURCE_DIR}/Code/Part2_range_tree/src/maple_tree_rcu_support")
add_library(MapleTree_lib STATIC ${MAPLETREE_DIR}/maple_tree.c )
target_link_libraries(MapleTree_lib PUBLIC urcu)

target_include_directories(MapleTree_lib PRIVATE ${MAPLETREE_DIR})