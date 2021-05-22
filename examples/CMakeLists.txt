cmake_minimum_required(VERSION 3.8)

project(idl-test
  LANGUAGES CXX
  DESCRIPTION "Example Ratatosk IDL usage")

set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler flag extension (use -std=c++11 instead of -std=gnu++11)
set(CMAKE_CXX_STANDARD 17) # The default CXX_STANDARD (resolves to a flag -std=c++17)..
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
else()
  message(FATAL_ERROR "No conan")
endif()

find_package(OpenSplice 6.9 REQUIRED)
find_package(RatatoskIDL CONFIG REQUIRED) # optionally specifying exact version

# Alternative 1:
# --------------------------------------------------------------------
# Extract the IDL files for a target as an absolute filename list
# --------------------------------------------------------------------
get_property(pingpong_idls
  TARGET RatatoskIDL::pingpong
  PROPERTY INTERFACE_SOURCES)
  #PROPERTY idl_sources) # You also need to acquire the install prefix
message(STATUS "Pingpong IDL files: ${pingpong_idls}")

# Generate DDS libraries by making use of ${pingpong_ids} file list

# Alternative 2:
# --------------------------------------------------------------------
# DDS code generation helper function
# The function assumes that OpenSplice_generate_isocpp2 is available,
# which may be provided with a FindOpenSplice.cmake
# --------------------------------------------------------------------

set(dds-lib pingpong-dds)
ratatosk_add_idl_library("${dds-lib}" pingpong)
add_library("${PROJECT_NAME}::${dds-lib}" ALIAS "${dds-lib}")


# ------------------------------------------------------
#  Example use of library
# ------------------------------------------------------

add_executable(test-ping ping.cpp)
target_link_libraries(test-ping pingpong-dds)

add_executable(test-pong pong.cpp)
target_link_libraries(test-pong pingpong-dds)
