# GammaRay-specific CMake macros that don't make sense outside of the GammaRay source tree.

# Copyright (c) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# All rights reserved.
#
# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Drop-in replacement for CMake's option()
# This version takes care of adding feature info to FeatureSummary for this option
#
# Usage:
#   gammaray_option(GAMMARAY_MULTI_BUILD "Build multiple applicable probe configurations." ON)
#
set(_gammaray_macros_internal_location ${CMAKE_CURRENT_LIST_DIR})

function(gammaray_option option description)
    set(extra_option_arguments ${ARGN})
    option(${option} "${description}" ${extra_option_arguments})
    add_feature_info("Option ${option}" ${option} "${description}")
endfunction()

macro(gammaray_target_relocatable_interfaces _paths)
  # See https://cmake.org/cmake/help/v3.3/manual/cmake-packages.7.html#creating-relocatable-packages
  get_filename_component(_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  # Allow include like:
  # #include <file.h>
  # #include <module/file.h>
  # #include <gammaray/module/file.h>
  set(${_paths}
      "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}/${_dir}"
      "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}"
      "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}/.."
    )
endmacro()

function(gammaray_install_headers)
  file(RELATIVE_PATH _dir ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  cmake_parse_arguments(HEADER_INSTALL "" "DESTINATION" "" ${ARGN})
  if(NOT HEADER_INSTALL_DESTINATION)
    set(HEADER_INSTALL_DESTINATION ${_dir})
  endif()
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(FILES ${HEADER_INSTALL_UNPARSED_ARGUMENTS} DESTINATION ${INCLUDE_INSTALL_DIR}/${HEADER_INSTALL_DESTINATION})
  endif()
  set_directory_properties(PROPERTIES GAMMARAY_INSTALLED_HEADERS "${HEADER_INSTALL_UNPARSED_ARGUMENTS}")

  get_property(_include_dirs GLOBAL PROPERTY GAMMARAY_HEADER_DIRS)
  list(APPEND _include_dirs "${_dir}")
  set_property(GLOBAL PROPERTY GAMMARAY_HEADER_DIRS "${_include_dirs}")
endfunction()

macro(gammaray_all_installed_headers _var)
  set(${_var} "")
  get_property(_include_dirs GLOBAL PROPERTY GAMMARAY_HEADER_DIRS)
  foreach(_dir ${_include_dirs})
    get_directory_property(_hdrs DIRECTORY ${CMAKE_SOURCE_DIR}/${_dir} GAMMARAY_INSTALLED_HEADERS)
    foreach(_hdr ${_hdrs})
      if(IS_ABSOLUTE ${_hdr})
        list(APPEND ${_var} ${_hdr})
      else()
        list(APPEND ${_var} "${CMAKE_SOURCE_DIR}/${_dir}/${_hdr}")
      endif()
    endforeach()
  endforeach()
endmacro()

macro(gammaray_join_list _var _sep)
  set(${_var} "")
  foreach(_element ${ARGN})
    set(${_var} "${${_var}}${_sep}\"${_element}\"")
  endforeach()
endmacro()

macro(gammaray_convert_to_relative_path _var)
  # Make sure _var is a relative path
  if(IS_ABSOLUTE "${${_var}}")
    file(RELATIVE_PATH ${_var} "${CMAKE_INSTALL_PREFIX}" "${${_var}}")
  endif()
endmacro()

macro(gammaray_inverse_dir _var _prefix)
  # strip out relative components, those break the following on OSX
  get_filename_component(_clean_prefix "${CMAKE_INSTALL_PREFIX}/${_prefix}" ABSOLUTE)
  file(RELATIVE_PATH ${_var} ${_clean_prefix} "${CMAKE_INSTALL_PREFIX}")
endmacro()

# embed an Info.plist file into a non-bundled Mac executable
macro(gammaray_embed_info_plist _target _plist)
  configure_file(${_plist} ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist)
  if(APPLE)
    set_target_properties(${_target} PROPERTIES LINK_FLAGS "-sectcreate __TEXT __info_plist ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist")
  endif()
endmacro()

macro(gammaray_add_win_icon _sources)
    if (WIN32)
        configure_file(${_gammaray_macros_internal_location}/gammaray.rc.cmake gammaray.rc)
        LIST(APPEND ${_sources} ${CMAKE_CURRENT_BINARY_DIR}/gammaray.rc)
    endif()
endmacro()

# allow to use CMake FeatureSummary with "packages" that consist only of a minor inline check
# rather than a fully-featured find module
macro(gammaray_add_dummy_package _package _found)
  if(${_found})
    set(_property_name "PACKAGES_FOUND")
  else()
    set(_property_name "PACKAGES_NOT_FOUND")
  endif()
  get_property(_packages GLOBAL PROPERTY ${_property_name})
  list(APPEND _packages ${_package})
  set_property(GLOBAL PROPERTY ${_property_name} "${_packages}")
endmacro()
