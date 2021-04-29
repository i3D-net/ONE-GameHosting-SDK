cmake_minimum_required(VERSION 3.10.2...3.17.2)

project(ONEGameHostingSDK
  VERSION 
  0.9.0 
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_MSVC_MT "Use Multi Threaded toolchain option instead of default Multi Threaded DLL" OFF)

if(WIN32)
  if(BUILD_MSVC_MT)
      add_compile_options(
          $<$<CONFIG:>:/MT>
          $<$<CONFIG:Debug>:/MTd>
          $<$<CONFIG:Release>:/MT>
      )
  endif()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N}")
  add_definitions(-DWINDOWS)
else()
  set(CMAKE_CXX_FLAGS -pthread)
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

add_subdirectory(one/arcus)
add_subdirectory(one/fake/agent)
add_subdirectory(one/fake/game)

set(PROJECT_ARCUS_PROTOCOL_VERSION V2)

configure_file(
  "${PROJECT_SOURCE_DIR}/arcus-version.h.in"
  "${PROJECT_SOURCE_DIR}/one/arcus/internal/version.h"
)

configure_file(
  "${PROJECT_SOURCE_DIR}/ping-version.h.in"
  "${PROJECT_SOURCE_DIR}/one/ping/internal/version.h"
)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING))
  set(RUN_TEST_AFTER_BUILD true)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
