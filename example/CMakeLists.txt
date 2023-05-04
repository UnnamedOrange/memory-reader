cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

add_subdirectory("example-01-process")
add_subdirectory("example-02-single-process-daemon")
add_subdirectory("example-03-signature")
add_subdirectory("example-04-offsets")
