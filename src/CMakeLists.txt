add_executable(10_1 1.cpp)
target_link_libraries(10_1 PRIVATE genetic_process ariel_random ${QOL_TARGETS})

add_executable(10_2 2.cpp)
target_link_libraries(10_2 PRIVATE genetic_process ariel_random ${QOL_TARGETS} ${MPI_TARGETS})