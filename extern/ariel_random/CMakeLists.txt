add_library(ariel_random SHARED ariel_random.cpp random_utils.hpp)
target_include_directories(ariel_random
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ariel_random PROPERTIES CXX_EXTENSIONS OFF)
