cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

file(
  GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "src/*"
)

add_executable(test-memory-reader)
target_compile_features(test-memory-reader PRIVATE cxx_std_20)
if(MSVC)
  target_compile_options(test-memory-reader PRIVATE /W4 /permissive /WX)
else()
  target_compile_options(test-memory-reader PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
target_sources(test-memory-reader PRIVATE "${SOURCES}")

target_link_libraries(test-memory-reader PRIVATE memory-reader)
target_link_libraries(test-memory-reader PRIVATE GTest::gtest GTest::gtest_main)

gtest_discover_tests(test-memory-reader)
