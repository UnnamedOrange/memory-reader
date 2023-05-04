cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

option(MEMORY_READER_BUILD_EXAMPLE "Build example" OFF)
option(MEMORY_READER_BUILD_TESTING "Build testing" OFF)
option(MEMORY_READER_BUILD_DOCUMENTS "Build documents" OFF)

project(memory-reader
  VERSION 0.3.0
  DESCRIPTION "Yet another memory reader in C++20 (or newer)."
  HOMEPAGE_URL "https://github.com/UnnamedOrange/memory-reader"
  LANGUAGES CXX
)

add_subdirectory("source")

if(MEMORY_READER_BUILD_EXAMPLE)
  add_subdirectory("example")
endif()

if(MEMORY_READER_BUILD_TESTING)
  enable_testing()
  add_subdirectory("test")
endif()

if(MEMORY_READER_BUILD_DOCUMENTS)
  find_package(Doxygen REQUIRED dot)
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_MARKDOWN_SUPPORT YES)
  set(DOXYGEN_AUTOLINK_SUPPORT YES)
  set(DOXYGEN_HAVE_DOT YES)
  set(DOXYGEN_COLLABORATION_GRAPH NO)
  set(DOXYGEN_CLASS_GRAPH YES)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_DOT_UML_DETAILS YES)
  set(DOXYGEN_DOT_WRAP_THRESHOLD 100)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_QUIET YES)

  doxygen_add_docs(
    docs
    "source"
    ALL
    COMMENT "Generating documentation with Doxygen."
  )
endif()
