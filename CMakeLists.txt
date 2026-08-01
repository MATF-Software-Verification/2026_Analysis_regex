cmake_minimum_required(VERSION 3.22)
project(RegexAnalysis)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_FUZZING "Enable libFuzzer build" OFF)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address -g)
        add_link_options(-fsanitize=address)
    endif()
endif()

set(BOOST_ROOT "" CACHE PATH "Path to Boost root dir")
set(Boost_NO_SYSTEM_PATHS TRUE CACHE BOOL "Do not search system Boost paths")
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "Do not use Boost's own CMake config") 

find_package(Boost REQUIRED COMPONENTS regex)

enable_testing()

add_executable(regex_app src/main.cpp)

target_link_libraries(regex_app PRIVATE Boost::regex)

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES 
    tests/test_basic_regex.cpp
    tests/test_match_results_sub_match.cpp
    tests/test_matching.cpp
    tests/test_replace.cpp
    tests/test_iterators.cpp
    tests/test_match_flags.cpp
    tests/test_groups.cpp
    tests/test_character_classes_and_word_boundaries.cpp
    tests/test_quantifiers_and_alternation.cpp
    tests/test_lookarounds.cpp
    tests/test_regex_compilation.cpp
    
)

add_executable(regex_tests ${TEST_SOURCES})

target_link_libraries(regex_tests PRIVATE 
    Boost::regex
    GTest::gtest 
    GTest::gtest_main
)

if(ENABLE_COVERAGE)
    message("Coverage enabled")
    target_compile_options(regex_tests PRIVATE --coverage)
    target_link_options(regex_tests PRIVATE --coverage)
endif()

add_test(NAME RegexTests COMMAND regex_tests)

