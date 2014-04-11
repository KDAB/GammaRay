# - Try to find Graphviz
# Once done this will define
#
#  GRAPHVIZ_FOUND - system has Graphviz
#  GRAPHVIZ_INCLUDE_DIR - the Graphviz include directory
#  GRAPHVIZ_LIBRARY - Link these to use Graphviz
#  GRAPHVIZ_VERSION = The value of PACKAGE_VERSION defined in graphviz_version.h
#  GRAPHVIZ_MAJOR_VERSION = The library major version number
#  GRAPHVIZ_MINOR_VERSION = The library minor version number
#  GRAPHVIZ_PATCH_VERSION = The library patch version number
#  GRAPHVIZ_COMPILE_FLAGS = List of compile flags needed by the GraphViz installation headers
#
# This module reads hints about search locations from the following env or cache variables:
#  GRAPHVIZ_ROOT          - Graphviz installation prefix
#                           (containing bin/, include/, etc.)

# Copyright (c) 2009, Adrien Bustany, <madcat@mymadcat.com>
# Copyright (c) 2013-2014 Kevin Funk <kevin.funk@kdab.com>

# Version computation and some cleanups by Allen Winter <allen.winter@kdab.com>
# Bug fixing for WIN32 by Guillaume Jacquenot <guillaume.jacquenot@gmail.com>
# Copyright (c) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Redistribution and use is allowed according to the terms of the GPLv3+ license.

include(CheckIncludeFiles)
include(CMakePushCheckState)

if(NOT GRAPHVIZ_MIN_VERSION)
  set(GRAPHVIZ_MIN_VERSION "2.00")
endif()

if(GRAPHVIZ_INCLUDE_DIR AND GRAPHVIZ_CDT_LIBRARY
    AND (GRAPHVIZ_CGRAPH_LIBRARY OR GRAPHVIZ_GRAPH_LIBRARY) AND GRAPHVIZ_PATHPLAN_LIBRARY)
  set(GRAPHVIZ_FIND_QUIETLY TRUE)
endif()

if (GRAPHVIZ_ROOT)
    set(_GRAPHVIZ_ROOT ${GRAPHVIZ_ROOT})
else()
    set(_GRAPHVIZ_ROOT $ENV{GRAPHVIZ_ROOT})
endif()

if(NOT _GRAPHVIZ_ROOT)
  if(WIN32)
      find_program(DOT_TOOL dot)
      get_filename_component(_GRAPHVIZ_ROOT ${DOT_TOOL} PATH)
  endif()
endif()

if(_GRAPHVIZ_ROOT)
  set(_GRAPHVIZ_INCLUDE_DIR ${_GRAPHVIZ_ROOT}/include)
  set(_GRAPHVIZ_LIBRARY_DIR ${_GRAPHVIZ_ROOT}/lib)
  set(_GRAPHVIZ_FIND_OPTS NO_DEFAULT_PATH)
else()
  set(_GRAPHVIZ_FIND_OPTS "")
endif()

find_path(GRAPHVIZ_INCLUDE_DIR NAMES graphviz/graph.h graphviz/cgraph.h
  HINTS ${_GRAPHVIZ_INCLUDE_DIR}
  ${_GRAPHVIZ_FIND_OPTS})

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(GRAPHVIZ_LIB_PATH_SUFFIX "release/lib")
  else()
    set(GRAPHVIZ_LIB_PATH_SUFFIX "debug/lib")
  endif()
else()
  set(GRAPHVIZ_LIB_PATH_SUFFIX)
endif()

find_library(GRAPHVIZ_CDT_LIBRARY NAMES cdt
  HINTS ${_GRAPHVIZ_LIBRARY_DIR} PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
  ${_GRAPHVIZ_FIND_OPTS})
find_library(GRAPHVIZ_GVC_LIBRARY NAMES gvc
  HINTS ${_GRAPHVIZ_LIBRARY_DIR} PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
  ${_GRAPHVIZ_FIND_OPTS})
find_library(GRAPHVIZ_CGRAPH_LIBRARY NAMES cgraph
  HINTS ${_GRAPHVIZ_LIBRARY_DIR} PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
  ${_GRAPHVIZ_FIND_OPTS})
find_library(GRAPHVIZ_GRAPH_LIBRARY NAMES graph
  HINTS ${_GRAPHVIZ_LIBRARY_DIR} PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
  ${_GRAPHVIZ_FIND_OPTS})
find_library(GRAPHVIZ_PATHPLAN_LIBRARY NAMES pathplan
  HINTS ${_GRAPHVIZ_LIBRARY_DIR} PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
  ${_GRAPHVIZ_FIND_OPTS})

cmake_push_check_state()
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${GRAPHVIZ_INCLUDE_DIR})
check_include_files(graphviz/graphviz_version.h HAVE_GRAPHVIZ_VERSION_H)
cmake_pop_check_state()

if(GRAPHVIZ_INCLUDE_DIR AND GRAPHVIZ_CDT_LIBRARY AND GRAPHVIZ_GVC_LIBRARY
    AND (GRAPHVIZ_CGRAPH_LIBRARY OR GRAPHVIZ_GRAPH_LIBRARY) AND GRAPHVIZ_PATHPLAN_LIBRARY)
  if(HAVE_GRAPHVIZ_VERSION_H OR WIN32)
    set(GRAPHVIZ_FOUND TRUE)
  endif()
else()
  set(GRAPHVIZ_FOUND FALSE)
endif()

# Ok, now compute the version and make sure its greater then the min required
if(GRAPHVIZ_FOUND)
  if(NOT WIN32)
    set(FIND_GRAPHVIZ_VERSION_SOURCE
      "#include <graphviz/graphviz_version.h>\n#include <stdio.h>\n int main()\n {\n printf(\"%s\",PACKAGE_VERSION);return 1;\n }\n")
    set(FIND_GRAPHVIZ_VERSION_SOURCE_FILE ${CMAKE_BINARY_DIR}/CMakeTmp/FindGRAPHVIZ.cxx)
    file(WRITE "${FIND_GRAPHVIZ_VERSION_SOURCE_FILE}" "${FIND_GRAPHVIZ_VERSION_SOURCE}")

    set(FIND_GRAPHVIZ_VERSION_ADD_INCLUDES
      "-DINCLUDE_DIRECTORIES:STRING=${GRAPHVIZ_INCLUDE_DIR}")

    if(NOT CMAKE_CROSSCOMPILING)
    try_run(RUN_RESULT COMPILE_RESULT
      ${CMAKE_BINARY_DIR}
      ${FIND_GRAPHVIZ_VERSION_SOURCE_FILE}
      CMAKE_FLAGS "${FIND_GRAPHVIZ_VERSION_ADD_INCLUDES}"
      RUN_OUTPUT_VARIABLE GRAPHVIZ_VERSION)
    endif()

    if(COMPILE_RESULT AND RUN_RESULT EQUAL 1 AND NOT CMAKE_CROSSCOMPILING)
      message(STATUS "Found Graphviz version ${GRAPHVIZ_VERSION}")
      if(${GRAPHVIZ_VERSION} VERSION_LESS ${GRAPHVIZ_MIN_VERSION})
        message(STATUS "Graphviz version ${GRAPHVIZ_VERSION} is too old. At least version ${GRAPHVIZ_MIN_VERSION} is needed.")
        set(GRAPHVIZ_FOUND FALSE)
        set(GRAPHVIZ_INCLUDE_DIR "")
        set(GRAPHVIZ_CDT_LIBRARY "")
        set(GRAPHVIZ_GVC_LIBRARY "")
        set(GRAPHVIZ_CGRAPH_LIBRARY "")
        set(GRAPHVIZ_GRAPH_LIBRARY "")
        set(GRAPHVIZ_PATHPLAN_LIBRARY "")
      else(${GRAPHVIZ_VERSION} VERSION_LESS ${GRAPHVIZ_MIN_VERSION})
        # Compute the major and minor version numbers
        if(NOT CMAKE_CROSSCOMPILING)
          string(REPLACE "." ";" VL ${GRAPHVIZ_VERSION})
          list(GET VL 0 GRAPHVIZ_MAJOR_VERSION)
          list(GET VL 1 GRAPHVIZ_MINOR_VERSION)
          list(GET VL 2 GRAPHVIZ_PATCH_VERSION)
        endif()
      endif(${GRAPHVIZ_VERSION} VERSION_LESS ${GRAPHVIZ_MIN_VERSION})
    else()
      if(NOT CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Unable to compile or run the graphviz version detection program.")
      endif()
    endif()
  elseif(WIN32)
    find_program(DOT_TOOL dot PATHS ${_GRAPHVIZ_ROOT}/bin)
    execute_process(COMMAND ${DOT_TOOL} -V OUTPUT_VARIABLE DOT_VERSION_OUTPUT ERROR_VARIABLE DOT_VERSION_OUTPUT OUTPUT_QUIET)
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" GRAPHVIZ_VERSION "${DOT_VERSION_OUTPUT}")
    string(REPLACE "." ";" VL ${GRAPHVIZ_VERSION})
    list(GET VL 0 GRAPHVIZ_MAJOR_VERSION)
    list(GET VL 1 GRAPHVIZ_MINOR_VERSION)
    list(GET VL 2 GRAPHVIZ_PATCH_VERSION)
  endif()

  set(GRAPHVIZ_COMPILE_FLAGS "")
  check_include_files(string.h HAVE_STRING_H)
  if (HAVE_STRING_H)
    list(APPEND GRAPHVIZ_COMPILE_FLAGS "-DHAVE_STRING_H=1")
  endif()

  if(NOT GRAPHVIZ_FIND_QUIETLY)
    message(STATUS "Found Graphviz: ${GRAPHVIZ_CDT_LIBRARY} ${GRAPHVIZ_GVC_LIBRARY} ${GRAPHVIZ_CGRAPH_LIBRARY} ${GRAPHVIZ_GRAPH_LIBRARY} ${GRAPHVIZ_PATHPLAN_LIBRARY}")
  endif()
else()
  if(GRAPHVIZ_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Graphivz")
  endif()
endif()
