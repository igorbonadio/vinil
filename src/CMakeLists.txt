cmake_minimum_required(VERSION 2.6)

add_library(vinil vhd.c vhd.h)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  target_link_libraries(vinil uuid)
ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")
