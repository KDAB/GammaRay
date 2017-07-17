# Copyright (c) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
  set(_build_target_dir "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}")

  # Work-around for KDEND-44 (also see https://cmake.org/Bug/bug_relationship_graph.php?bug_id=15419)
  # Re-generates moc file in case the JSON file changes
  if (Qt5Core_FOUND AND NOT Qt5Core_VERSION VERSION_LESS 5.6.0) # DEPENDS argument for qt5_wrap_cpp was added in 5.6.0
      list(GET _gammaray_add_plugin_SOURCES 0 mainSourceFile)
      string(REPLACE ".cpp" ".h" mainHeaderFile ${mainSourceFile})

      # sanity check
      file(READ ${mainHeaderFile} mainHeaderFileContents)
      string(FIND "${mainHeaderFileContents}" "Q_PLUGIN_METADATA" hasPluginMetadataMacroMatchRes)
      if (hasPluginMetadataMacroMatchRes EQUAL -1)
          message(FATAL_ERROR "First file passed to SOURCES must be the .cpp file which includes the header using the Q_PLUGIN_METADATA macro")
      endif()

      qt5_wrap_cpp(_gammaray_add_plugin_SOURCES ${mainHeaderFile} DEPENDS ${_gammaray_add_plugin_JSON} TARGET ${_target_name})
      set_source_files_properties("${mainHeaderFile}" PROPERTIES SKIP_AUTOMOC TRUE)
      set_source_files_properties("${mainSourceFile}" PROPERTIES SKIP_AUTOMOC TRUE)
      # workaround AUTOUIC failing on files with SKIP_AUTOMOC enabled
      string(REPLACE ".cpp" ".ui" mainUiFile ${mainSourceFile})
      if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${mainUiFile}")
          qt5_wrap_ui(_gammaray_add_plugin_SOURCES ${mainUiFile})
      endif()
  endif()

  add_library(${_target_name} ${GAMMARAY_PLUGIN_TYPE} ${_gammaray_add_plugin_SOURCES})
  set_target_properties(${_target_name} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${_build_target_dir}
  )
  if(NOT ANDROID)
    set_target_properties(${_target_name} PROPERTIES PREFIX "")
  else()
    set_target_properties(${_target_name} PROPERTIES PREFIX "libplugins_gammaray_")
  endif()
  if(GAMMARAY_STATIC_PROBE)
    set_target_properties(${_target_name} PROPERTIES COMPILE_DEFINITIONS QT_STATICPLUGIN)
  endif()

  if(GAMMARAY_INSTALL_QT_LAYOUT)
    set_target_properties(${_target_name} PROPERTIES OUTPUT_NAME ${_target_name}-${GAMMARAY_PROBE_ABI})
  endif()

  if(APPLE)
    if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
      set_target_properties(${_target_name} PROPERTIES INSTALL_RPATH "@loader_pathd/../../../../Frameworks")
    endif()
  endif()

  install(TARGETS ${_target_name} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
  if(NOT ${Qt5Core_FOUND} AND _gammaray_add_plugin_DESKTOP)
    set(plugin_exec ${_target_name}${GAMMARAY_PROBE_ABI_POSTFIX})
    if(GAMMARAY_INSTALL_QT_LAYOUT)
      set(plugin_exec ${_target_name}-${GAMMARAY_PROBE_ABI}${GAMMARAY_PROBE_ABI_POSTFIX})
    endif()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_gammaray_add_plugin_DESKTOP}" "${_build_target_dir}/${plugin_exec}.desktop")
    install(FILES "${_build_target_dir}/${plugin_exec}.desktop" DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
  endif()
  if(MSVC)
    install(FILES "$<TARGET_PDB_FILE_DIR:${_target_name}>/$<TARGET_PDB_FILE_NAME:${_target_name}>" DESTINATION ${PROBE_PLUGIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
  endif()
endmacro()
