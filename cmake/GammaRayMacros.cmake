# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

include(CMakeParseArguments)

#
# GAMMARAY_ADD_PLUGIN: create a gammaray plugin, install at the right place, etc
# arguments:
# - JSON <file> - the plugin .json file
# - SOURCES <files> - the plugin sources
#
macro(gammaray_add_plugin _target_name)
    set(oneValueArgs JSON)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(
        _gammaray_add_plugin
        ""
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT PROBE_PLUGIN_INSTALL_DIR) # HACK for external plugins that don't set PLUGIN_INSTALL_DIR
        set(PROBE_PLUGIN_INSTALL_DIR ${GAMMARAY_PROBE_PLUGIN_INSTALL_DIR})
    endif()
    set(_build_target_dir "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}")

    # Work-around for KDEND-44 (also see https://cmake.org/Bug/bug_relationship_graph.php?bug_id=15419)
    # Re-generates moc file in case the JSON file changes
    # DEPENDS argument for qt5_wrap_cpp was added in 5.6.0
    # CMake 3.9 or higher (which is required by Qt 6) does all that correctly
    if(Qt5Core_FOUND
       AND NOT Qt5Core_VERSION VERSION_LESS 5.6.0
       AND CMAKE_VERSION VERSION_LESS 3.9
    )
        list(GET _gammaray_add_plugin_SOURCES 0 mainSourceFile)
        string(REPLACE ".cpp" ".h" mainHeaderFile ${mainSourceFile})

        # sanity check
        file(READ ${mainHeaderFile} mainHeaderFileContents)
        string(FIND "${mainHeaderFileContents}" "Q_PLUGIN_METADATA" hasPluginMetadataMacroMatchRes)
        if(hasPluginMetadataMacroMatchRes EQUAL -1)
            message(FATAL_ERROR "First file passed to SOURCES must be the .cpp file which includes "
                                "the header using the Q_PLUGIN_METADATA macro"
            )
        endif()

        qt5_wrap_cpp(
            _gammaray_add_plugin_SOURCES
            ${mainHeaderFile}
            DEPENDS
            ${_gammaray_add_plugin_JSON}
            TARGET
            ${_target_name}
        )
        set_source_files_properties("${mainHeaderFile}" PROPERTIES SKIP_AUTOMOC TRUE)
        set_source_files_properties("${mainSourceFile}" PROPERTIES SKIP_AUTOMOC TRUE)
        set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/moc_${mainSourceFile} PROPERTY SKIP_AUTOGEN ON)
        # workaround AUTOUIC failing on files with SKIP_AUTOMOC enabled
        string(REPLACE ".cpp" ".ui" mainUiFile ${mainSourceFile})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${mainUiFile}")
            qt5_wrap_ui(_gammaray_add_plugin_SOURCES ${mainUiFile})
            set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ui_${mainHeaderFile} PROPERTY SKIP_AUTOGEN ON)
        endif()
    endif()

    add_library(${_target_name} ${GAMMARAY_PLUGIN_TYPE} ${_gammaray_add_plugin_SOURCES})
    set_target_properties(${_target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${_build_target_dir})
    set_target_properties(${_target_name} PROPERTIES PREFIX "")
    if(GAMMARAY_STATIC_PROBE)
        set_target_properties(${_target_name} PROPERTIES COMPILE_DEFINITIONS QT_STATICPLUGIN)
    endif()

    if(GAMMARAY_INSTALL_QT_LAYOUT)
        set_target_properties(${_target_name} PROPERTIES OUTPUT_NAME ${_target_name}-${GAMMARAY_PROBE_ABI})
    endif()
    gammaray_set_rpath(${_target_name} ${PROBE_PLUGIN_INSTALL_DIR})

    install(TARGETS ${_target_name} DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
    if(MSVC)
        install(
            FILES "$<TARGET_PDB_FILE_DIR:${_target_name}>/$<TARGET_PDB_FILE_NAME:${_target_name}>"
            DESTINATION ${PROBE_PLUGIN_INSTALL_DIR}
            CONFIGURATIONS Debug RelWithDebInfo
        )
    endif()

    if(GAMMARAY_USE_PCH)
        target_precompile_headers(${_target_name} REUSE_FROM gammaray_pch_core_gui)
    endif()
endmacro()

# Common RPATH setup
# @internal
function(gammaray_set_rpath _target_name _install_prefix)
    get_filename_component(_clean_prefix "${CMAKE_INSTALL_PREFIX}/${_install_prefix}" ABSOLUTE)
    file(RELATIVE_PATH _relative_rpath ${_clean_prefix} "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
    if(UNIX AND NOT APPLE)
        set(_rpath "\$ORIGIN/${_relative_rpath}")
        if(CMAKE_INSTALL_RPATH)
            list(APPEND _rpath ${CMAKE_INSTALL_RPATH})
        endif()
        set_target_properties(${_target_name} PROPERTIES INSTALL_RPATH "${_rpath}")
    endif()
    if(APPLE)
        set_target_properties(${_target_name} PROPERTIES INSTALL_RPATH "@loader_path/${_relative_rpath}")
    endif()
endfunction()
