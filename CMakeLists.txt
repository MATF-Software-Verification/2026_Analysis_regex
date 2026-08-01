cmake_minimum_required(VERSION 3.22)
project(RegexAnalysis)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_SANITIZERS "Enable Address and Underfined Behaviour Sanitizer" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_FUZZING "Enable libFuzzer build" OFF)

set(BOOST_ROOT "" CACHE PATH "Path to Boost root dir")
set(Boost_NO_SYSTEM_PATHS TRUE CACHE BOOL "Do not search system Boost paths")
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "Do not use Boost's own CMake config") 

find_package(Boost REQUIRED COMPONENTS regex)

enable_testing()

add_executable(regex_app src/main.cpp)

target_link_libraries(regex_app PRIVATE Boost::regex)

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES 
    unit_tests/test_basic_regex.cpp
    unit_tests/test_match_results_sub_match.cpp
    unit_tests/test_matching.cpp
    unit_tests/test_replace.cpp
    unit_tests/test_iterators.cpp
    unit_tests/test_match_flags.cpp
    unit_tests/test_groups.cpp
    unit_tests/test_character_classes_and_word_boundaries.cpp
    unit_tests/test_quantifiers_and_alternation.cpp
    unit_tests/test_lookarounds.cpp
    unit_tests/test_regex_compilation.cpp
    
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

if(ENABLE_SANITIZERS)
    set(SANITIZER_FLAGS
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
        -O0
    )

    target_compile_options(regex_app PRIVATE ${SANITIZER_FLAGS})
    target_link_options(regex_app PRIVATE -fsanitize=address,undefined)

    target_compile_options(regex_tests PRIVATE ${SANITIZER_FLAGS})
    target_link_options(regex_tests PRIVATE -fsanitize=address,undefined)
endif()


add_test(NAME RegexTests COMMAND regex_tests)

