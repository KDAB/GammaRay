#
# GAMMARAY_ADD_PLUGIN: create a gammaray plugin, install at the right place, etc
#

#  Copyright (c) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

macro(gammaray_add_plugin _target_name _desktop_file)
  if(NOT PROBE_PLUGIN_INSTALL_DIR) # HACK for external plugins that don't set PLUGIN_INSTALL_DIR
    set(PROBE_PLUGIN_INSTALL_DIR ${GAMMARAY_PROBE_PLUGIN_INSTALL_DIR})
  endif()
  set(_build_target_dir "${PROJECT_BINARY_DIR}/${PROBE_PLUGIN_INSTALL_DIR}")

  add_library(${_target_name} MODULE ${ARGN})
  set_target_properties(${_target_name} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${_build_target_dir}
  )

  if(APPLE)
    set_target_properties(${_target_name} PROPERTIES INSTALL_RPATH "@loader_path/../../../Frameworks")
  endif()

  install(TARGETS ${_target_name} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
  if(NOT ${Qt5Core_FOUND})
    install(FILES ${_desktop_file} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_desktop_file}" "${_build_target_dir}/${_desktop_file}")
  endif()
endmacro()
