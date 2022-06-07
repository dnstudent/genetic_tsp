cmake_minimum_required(VERSION 3.20)
project(genetic_tsp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)

option(USE_MPI "Use mpi" ON)
if (USE_MPI)
    add_definitions(-DUSE_MPI)
endif ()

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings
        INTERFACE
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -pedantic
        -Wpedantic
        -Wconversion
        #-Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Werror)

if (USE_MPI)
    find_package(MPI REQUIRED)
    set(MPI_TARGETS MPI::MPI_CXX)
else ()
    set(MPI_TARGETS "")
endif ()

find_package(Catch2 CONFIG REQUIRED)
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_package(indicators CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

add_library(rapidcsv INTERFACE)
target_include_directories(rapidcsv INTERFACE ${RAPIDCSV_INCLUDE_DIRS})

list(APPEND QOL_TARGETS cxxopts::cxxopts indicators::indicators rapidcsv project_warnings)

configure_file(
        "${PROJECT_SOURCE_DIR}/src/include/config.hpp.in"
        "${PROJECT_SOURCE_DIR}/src/include/config.hpp")

include_directories("${PROJECT_SOURCE_DIR}/src/include")

add_subdirectory(extern)
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)