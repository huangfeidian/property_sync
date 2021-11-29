find_package(meta REQUIRED)
add_executable(generate_property ${CMAKE_CURRENT_SOURCE_DIR}/generate_property.cpp)
target_link_libraries(generate_property meta)