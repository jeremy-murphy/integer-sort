cmake_minimum_required(VERSION 2.8)
project(integer_sort)
include_directories("." "../algorithm_sorting/")
add_executable(integer_sort libs/algorithm/sorting/test/test.cpp)
target_link_libraries(integer_sort rt)

install(TARGETS integer_sort RUNTIME DESTINATION bin)
