cmake_minimum_required(VERSION 3.27)
project(RegexAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address -g)
        add_link_options(-fsanitize=address)
    endif()
endif()

set(BOOST_ROOT "" CACHE PATH "Path to Boost root dir")
set(Boost_LIBRARY_DIRS "" CACHE PATH "Path to Boost libraries")
set(Boost_NO_SYSTEM_PATHS TRUE)
set(Boost_NO_BOOST_CMAKE ON)

set(VCPKG_MANIFEST_NO_DEFAULT_FEATURES ON)

find_package(Boost REQUIRED COMPONENTS regex)

add_executable(regex_app src/main.cpp)

target_link_libraries(regex_app PRIVATE Boost::regex)

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES 
    tests/test_basic_matching.cpp
    tests/test_flags.cpp
    tests/test_groups.cpp
    tests/test_iterators.cpp
    tests/test_character_classes_and_literals.cpp
    tests/test_quantifiers_and_alternation.cpp
    tests/test_lookarounds.cpp
    tests/test_regex_compilation.cpp
    tests/test_match_flags_extended.cpp
    tests/test_replace.cpp
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE 
    Boost::regex
    GTest::gtest 
    GTest::gtest_main
)

enable_testing()
add_test(NAME RegexTests COMMAND tests)