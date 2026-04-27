cmake_minimum_required(VERSION 3.16)
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

set(BOOST_REGEX_INCLUDE_DIR "C:/matf/boost")
set(BOOST_REGEX_LIB "C:/matf/boost/stage/lib")

include_directories(${BOOST_REGEX_INCLUDE_DIR})
link_directories(${BOOST_REGEX_LIB})


add_executable(regex_app src/main.cpp)

target_link_libraries(regex_app PRIVATE "${BOOST_REGEX_LIB}/libboost_regex-vc145-mt-gd-x64-1_92.lib")

find_package(GTest CONFIG REQUIRED)

set(TEST_SOURCES 
    tests/test_basic_matching.cpp
    tests/test_groups.cpp
)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE 
    "${BOOST_REGEX_LIB}/libboost_regex-vc145-mt-gd-x64-1_92.lib"
    GTest::gtest 
    GTest::gtest_main
)

enable_testing()

add_test(NAME RegexTests COMMAND tests)