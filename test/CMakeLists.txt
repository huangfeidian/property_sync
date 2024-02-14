include_directories(${CMAKE_CURRENT_SOURCE_DIR}/generated)
FILE(GLOB TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(property_test ${TEST_SRCS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated/)
target_link_libraries(property_test any_container)