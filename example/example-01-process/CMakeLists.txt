cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

set(EXAMPLE_NAME example-01-process)

add_executable(${EXAMPLE_NAME} "main.cpp")
target_compile_features(${EXAMPLE_NAME} PRIVATE cxx_std_20)
if(MSVC)
  target_compile_options(${EXAMPLE_NAME} PRIVATE /W4 /permissive /WX)
else()
  target_compile_options(${EXAMPLE_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(${EXAMPLE_NAME} PRIVATE memory-reader)
