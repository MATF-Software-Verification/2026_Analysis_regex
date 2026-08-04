cmake_minimum_required(VERSION 3.22)
project(RegexFuzzer LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "LibFuzzer requires Clang. Set CXX=clang++ in fuzz.cfg")
endif()

add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++ -lc++abi -fuse-ld=lld)

set(BOOST_ROOT "" CACHE PATH "Path to Boost root dir")
set(Boost_NO_SYSTEM_PATHS TRUE CACHE BOOL "Do not search system Boost paths")
set(Boost_NO_BOOST_CMAKE ON CACHE BOOL "Do not use Boost's own CMake config")

find_package(Boost REQUIRED COMPONENTS regex)

include(FetchContent)
FetchContent_Declare(
    fuzztest
    GIT_REPOSITORY https://github.com/google/fuzztest.git
    GIT_TAG 1c9fe96505ae027ac0a5a06c2538402f90dc30c3
)
FetchContent_MakeAvailable(fuzztest)

fuzztest_setup_fuzzing_flags()

enable_testing()

include(GoogleTest)
 
add_executable(fuzz_regex fuzz_regex.cc)

target_link_libraries(fuzz_regex PRIVATE Boost::regex)

link_fuzztest(fuzz_regex)

gtest_discover_tests(fuzz_regex)


