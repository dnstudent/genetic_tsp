add_executable(genetic_tsp main.cpp)
target_link_libraries(genetic_tsp xtensor xtensor::optimize xtensor::use_xsimd)