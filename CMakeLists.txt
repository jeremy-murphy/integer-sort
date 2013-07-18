project(integer_sort)
include_directories(".")
add_executable(integer_sort main.cpp)

install(TARGETS integer_sort RUNTIME DESTINATION bin)
