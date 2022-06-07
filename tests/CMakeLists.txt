add_executable(tests TestCatch.cpp TestShuffle.cpp TestUtils.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain genetic_process lcg ariel_random)
target_compile_features(tests PUBLIC cxx_std_20)

include(Catch)
catch_discover_tests(tests)