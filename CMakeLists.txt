cmake_minimum_required(VERSION 3.16)
project(RegexAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

find_package(Boost REQUIRED COMPONENTS regex)
find_package(GTest REQUIRED)

add_executable(regex_app src/main.cpp)

target_link_libraries(regex_app PRIVATE Boost::regex)

set(TEST_SOURCES 
    tests/test_basic_matching.cpp
    tests/test_groups.cpp
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE 
    Boost::regex
    GTest::GTest 
    GTest::Main
)

enable_testing()

add_test(NAME RegexTests COMMAND tests)