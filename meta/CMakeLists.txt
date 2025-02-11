project(generate_property_sync)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(MSVC)
find_package(LLVM REQUIRED)

# add LLVM includes
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIR})
message(STATUS "libclang lib is " ${LIBCLANG_LIBRARY})
else()
find_package(Clang REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
link_directories(${CLANG_LIBRARY_DIR})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
set(LIBCLANG_LIBRARY libclang)
endif(MSVC)
find_package(meta REQUIRED)
add_executable(generate_property_sync ${CMAKE_CURRENT_SOURCE_DIR}/generate_property_sync.cpp)
target_link_libraries(generate_property_sync  meta)
