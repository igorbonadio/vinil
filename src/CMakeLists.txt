cmake_minimum_required(VERSION 2.6)

add_library(vinil SHARED vhd.c vhd.h crossplatform.c crossplatform.h)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  target_link_libraries(vinil uuid)
ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")

install(TARGETS vinil
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static )

install(FILES util.h vhd.h crossplatform.h DESTINATION include/vinil)