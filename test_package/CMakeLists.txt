cmake_minimum_required(VERSION 3.13)
project(RatatoskIDLTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
include("${CMAKE_BINARY_DIR}/conan_paths.cmake")


find_package(OpenSplice 6.9 REQUIRED)
find_package(RatatoskIDL CONFIG REQUIRED) # optionally specifying exact version

ratatosk_add_idl_library(ping-dds pingpong)

add_executable(ratatoskIdlTest ratatoskIdlTest.cpp)
target_link_libraries(ratatoskIdlTest ping-dds)

enable_testing()

add_test(NAME ratatoskIdlTest COMMAND ratatoskIdlTest)
