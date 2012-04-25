cmake_minimum_required(VERSION 2.6)

include_directories(../src/)

add_executable(check_vhd check_vhd.c)
target_link_libraries(check_vhd check vinil)
add_test(check_vhd check_vhd)

enable_testing()