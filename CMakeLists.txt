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
find_package(nlohmann_json CONFIG REQUIRED)


find_package(any_container CONFIG REQUIRED)

INCLUDE_DIRECTORIES(${any_container_include_dirs})
message(STATUS "any_container_include_dirs=${any_container_include_dirs}")
add_executable(property_sync ${CMAKE_SOURCE_DIR}/test/property_test.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC any_container)