cmake_minimum_required(VERSION 3.10)
project(check_point_format LANGUAGES CXX)

find_package(PDAL REQUIRED)

add_executable(check_point_format check_point_format.cpp)
target_compile_features(check_point_format PRIVATE cxx_std_17)
target_link_libraries(check_point_format PRIVATE PDAL::pdal)