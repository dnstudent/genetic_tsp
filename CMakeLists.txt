cmake_minimum_required(VERSION 3.22)
project(genetic_tsp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED )

find_package(catch2 CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)

add_subdirectory(extern)
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)