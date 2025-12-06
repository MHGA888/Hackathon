cmake_minimum_required(VERSION 3.13)
project(WritinglasToPcd)

find_package(PDAL 2.0.0 REQUIRED CONFIG)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lasToPcd lasToPcd.cpp)

target_link_libraries(lasToPcd PRIVATE ${PDAL_LIBRARIES})
target_include_directories(lasToPcd PRIVATE
    ${PDAL_INCLUDE_DIRS}
    ${PDAL_INCLUDE_DIRS}/pdal)