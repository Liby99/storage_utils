cmake_minimum_required(VERSION 3.10)

# Basic project setup
project(storage-utils CXX)

# CXX setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Basic
add_library(storage-utils INTERFACE)
target_include_directories(storage-utils INTERFACE include/)

# Testing setup
enable_testing()
file(GLOB_RECURSE test_files "tests/**.cpp")
foreach(test_file ${test_files})
  get_filename_component(test_target ${test_file} NAME_WE)
  add_executable(${test_target} ${test_file})
  target_include_directories(${test_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  add_test(${test_target} ${test_target})
endforeach()