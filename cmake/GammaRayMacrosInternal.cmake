# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# Drop-in replacement for CMake's option()
# This version takes care of adding feature info to FeatureSummary for this option
#
# Usage:
#   gammaray_option(GAMMARAY_MULTI_BUILD "Build multiple applicable probe configurations." ON)
#
set(_gammaray_macros_internal_location ${CMAKE_CURRENT_LIST_DIR})

# option() wrapper that adds feature info
function(gammaray_option option description)
    set(extra_option_arguments ${ARGN})
    option(${option} "${description}" ${extra_option_arguments})
    add_feature_info("Option ${option}" ${option} "${description}")
endfunction()

# From the CMAKE_CURRENT_SOURCE_DIR, compute the relocatable interfaces into the _paths variable
macro(gammaray_target_relocatable_interfaces _paths)
    # See https://cmake.org/cmake/help/v3.3/manual/cmake-packages.7.html#creating-relocatable-packages
    get_filename_component(_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    # Allow include like:
    # #include <file.h>
    # #include <module/file.h>
    # #include <gammaray/module/file.h>
    set(${_paths} "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}/${_dir}" "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}"
                  "$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}/.."
    )
endmacro()

# Append to the global GAMMARY_HEADER_DIRS property for the provided files list
function(gammaray_install_headers)
    file(RELATIVE_PATH _dir ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    cmake_parse_arguments(
        HEADER_INSTALL
        ""
        "DESTINATION"
        ""
        ${ARGN}
    )
    if(NOT HEADER_INSTALL_DESTINATION)
        set(HEADER_INSTALL_DESTINATION ${_dir})
    endif()
    if(NOT GAMMARAY_PROBE_ONLY_BUILD)
        install(FILES ${HEADER_INSTALL_UNPARSED_ARGUMENTS}
                DESTINATION ${INCLUDE_INSTALL_DIR}/${HEADER_INSTALL_DESTINATION}
        )
    endif()
    set_directory_properties(PROPERTIES GAMMARAY_INSTALLED_HEADERS "${HEADER_INSTALL_UNPARSED_ARGUMENTS}")

    get_property(_include_dirs GLOBAL PROPERTY GAMMARAY_HEADER_DIRS)
    list(APPEND _include_dirs "${_dir}")
    set_property(GLOBAL PROPERTY GAMMARAY_HEADER_DIRS "${_include_dirs}")
endfunction()

# Make a list of all installed headers
# For each directory in the global GAMMARRAY_HEADER_DIRS, append from the GAMMARAY_INSTALLED_HEADER property
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

# Join each provided command line item into a list, joined by the specified separator
macro(gammaray_join_list _var _sep)
    set(${_var} "")
    foreach(_element ${ARGN})
        set(${_var} "${${_var}}${_sep}\"${_element}\"")
    endforeach()
endmacro()

# Compute relative path (relative to the CMAKE_INSTALL_PREFIX).
macro(gammaray_convert_to_relative_path _var)
    # Make sure _var is a relative path
    if(IS_ABSOLUTE "${${_var}}")
        file(RELATIVE_PATH ${_var} "${CMAKE_INSTALL_PREFIX}" "${${_var}}")
    endif()
endmacro()

# Inverse the specified path to the provided prefix
macro(gammaray_inverse_dir _var _prefix)
    # strip out relative components, those break the following on OSX
    get_filename_component(_clean_prefix "${CMAKE_INSTALL_PREFIX}/${_prefix}" ABSOLUTE)
    file(RELATIVE_PATH ${_var} ${_clean_prefix} "${CMAKE_INSTALL_PREFIX}")
endmacro()

# embed an Info.plist file into a non-bundled Mac executable
macro(gammaray_embed_info_plist _target _plist)
    configure_file(${_plist} ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist)
    if(APPLE)
        set_target_properties(
            ${_target} PROPERTIES LINK_FLAGS
                                  "-sectcreate __TEXT __info_plist ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist"
        )
    endif()
endmacro()

# Add resource for the Windows icon
macro(gammaray_add_win_icon _sources)
    if(WIN32)
        configure_file(${_gammaray_macros_internal_location}/gammaray.rc.cmake gammaray.rc)
        list(APPEND ${_sources} ${CMAKE_CURRENT_BINARY_DIR}/gammaray.rc)
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

# Check if some required submodule is initialized
function(gammaray_ensure_submodule_exists submodule)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${submodule}/.git")
        if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
            message(
                FATAL_ERROR
                    "The git submodule ${submodule} is not initialized.\n"
                    "Please run the following command in the source directory (${CMAKE_SOURCE_DIR}):\n"
                    "    git submodule update --init --recursive ${CMAKE_CURRENT_SOURCE_DIR}/${submodule}\n"
            )
        else()
            message(FATAL_ERROR "The submodules are missing - please report a broken source package.\n")
        endif()
    endif()
endfunction()
