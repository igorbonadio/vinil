cmake_minimum_required(VERSION 2.6)

project(vinil)

add_subdirectory(src)
add_subdirectory(tests)

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY 
                    ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" 
                    VERBATIM)
endif(DOXYGEN_FOUND)

enable_testing()