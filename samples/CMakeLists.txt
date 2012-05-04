cmake_minimum_required(VERSION 2.6)

include_directories(../src/)

add_executable(create_vhd create_vhd.c)
target_link_libraries(create_vhd vinil)

add_executable(write_vhd write_vhd.c)
target_link_libraries(write_vhd vinil)

add_executable(read_vhd read_vhd.c)
target_link_libraries(read_vhd vinil)

