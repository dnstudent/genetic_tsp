add_library(genetic_process INTERFACE
        genetic_process.hpp
        )
target_link_libraries(genetic_process INTERFACE ariel_random project_warnings indicators::indicators ${MPI_TARGETS})
target_include_directories(genetic_process INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
