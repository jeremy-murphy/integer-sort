cmake_minimum_required(VERSION 2.8)
project(integer_sort)
include_directories(".")
add_executable(integer_sort main.cpp)

install(TARGETS integer_sort RUNTIME DESTINATION bin)
