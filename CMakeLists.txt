cmake_minimum_required(VERSION 3.15)
project(sorted)

set(CMAKE_CXX_STANDARD 11)

add_executable(test_insert test_insert.cc)
add_executable(test_erase test_erase.cc)