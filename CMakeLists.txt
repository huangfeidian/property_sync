CMAKE_MINIMUM_REQUIRED(VERSION 3.4)

PROJECT(property_sync)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-static ${CMAKE_CXX_FLAGS}")
if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/generated)

FILE(GLOB_RECURSE PROP_SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE PROP_GENERATED_SRCS ${CMAKE_SOURCE_DIR}/generated/*.cpp)

find_package(nlohmann_json CONFIG REQUIRED)


find_package(any_container CONFIG REQUIRED)

INCLUDE_DIRECTORIES(${any_container_include_dirs})

add_library(${CMAKE_PROJECT_NAME} ${PROP_SRCS}  ${PROP_GENERATED_SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC any_container)

message(STATUS "any_container_include_dirs=${any_container_include_dirs}")
add_subdirectory(test)
add_subdirectory(meta)
