find_package(meta REQUIRED)
find_package(Clang REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
link_directories(${CLANG_LIBRARY_DIR})
add_definitions(${CLANG_DEFINITIONS})
set(LIBCLANG_LIBRARY libclang)
add_executable(generate_property ${CMAKE_CURRENT_SOURCE_DIR}/generate_property.cpp)
target_link_libraries(generate_property meta)