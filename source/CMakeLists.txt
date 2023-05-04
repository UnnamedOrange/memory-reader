cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

file(
  GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "src/*"
  "inc/*"
)

add_library(memory-reader STATIC)
target_compile_features(memory-reader PRIVATE cxx_std_20)
if(MSVC)
  target_compile_options(memory-reader PRIVATE /W4 /permissive /WX)
else()
  target_compile_options(memory-reader PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
target_include_directories(memory-reader PRIVATE "inc/memory-reader")
target_include_directories(memory-reader PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>")
set_target_properties(memory-reader
    PROPERTIES PUBLIC_HEADER
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>/")
target_sources(memory-reader PRIVATE "${SOURCES}")

target_compile_definitions(memory-reader PRIVATE
  MEMORY_READER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  MEMORY_READER_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  MEMORY_READER_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  target_compile_definitions(memory-reader PUBLIC MEMORY_READER_TARGET_PLATFORM_WIN32=1)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
  target_compile_definitions(memory-reader PUBLIC MEMORY_READER_TARGET_PLATFORM_LINUX=1)
else()
  message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

include(GNUInstallDirs)
install(TARGETS memory-reader
  EXPORT memory-reader-export
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT memory-reader-export
  FILE "memory-reader-config.cmake"
  DESTINATION "cmake"
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "memory-reader-config-version.cmake"
  COMPATIBILITY SameMajorVersion
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/memory-reader-config-version.cmake"
  DESTINATION "cmake"
)
