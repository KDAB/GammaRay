#  Copyright (c) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

include(CMakeParseArguments)

#
# GAMMARAY_ADD_PLUGIN: create a gammaray plugin, install at the right place, etc
# arguments:
# - DESKTOP <file> - the Qt4 plugin .desktop file (optional)
# - JSON <file> - the Qt5 plugin .json file (optional)
# - SOURCES <files> - the plugin sources
#
macro(gammaray_add_plugin _target_name)
  set(oneValueArgs JSON DESKTOP)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(_gammaray_add_plugin "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT PROBE_PLUGIN_INSTALL_DIR) # HACK for external plugins that don't set PLUGIN_INSTALL_DIR
    set(PROBE_PLUGIN_INSTALL_DIR ${GAMMARAY_PROBE_PLUGIN_INSTALL_DIR})
  endif()
  set(_build_target_dir "${PROJECT_BINARY_DIR}/${PROBE_PLUGIN_INSTALL_DIR}")

  add_library(${_target_name} MODULE ${_gammaray_add_plugin_SOURCES})
  set_target_properties(${_target_name} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${_build_target_dir}
  )

  if(GAMMARAY_INSTALL_QT_LAYOUT)
    set_target_properties(${_target_name} PROPERTIES OUTPUT_NAME ${_target_name}-${GAMMARAY_PROBE_ABI})
  endif()

  # as of CMake 3.3 plugin JSON files are not automatically added as dependency
  if(${Qt5Core_FOUND} AND _gammaray_add_plugin_JSON)
    get_filename_component(_json "${_gammaray_add_plugin_JSON}" REALPATH)
    set_property(TARGET ${_target_name} APPEND PROPERTY AUTOGEN_TARGET_DEPENDS ${_json})
  endif()

  if(APPLE)
    if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
      set_target_properties(${_target_name} PROPERTIES INSTALL_RPATH "@loader_path/../../../Frameworks")
    endif()
  endif()

  install(TARGETS ${_target_name} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
  if(NOT ${Qt5Core_FOUND} AND _gammaray_add_plugin_DESKTOP)
    install(FILES ${_gammaray_add_plugin_DESKTOP} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_gammaray_add_plugin_DESKTOP}" "${_build_target_dir}/${_gammaray_add_plugin_DESKTOP}")
  endif()
endmacro()
