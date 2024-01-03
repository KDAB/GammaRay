# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

set(QmlLint_EXECUTABLE)
set(QmlLint_FOUND FALSE)

# First check for a target (Qt6)
if(TARGET Qt${QT_VERSION_MAJOR}::qmllint)
    set(QmlLint_FOUND TRUE)
    set(QmlLint_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmllint)
else()
    # See if it's on the PATH
    find_program(QmlLint_EXECUTABLE qmllint)
    if(QmlLint_EXECUTABLE)
        if(NOT QMLLINT_IS_WORKING)
            # Try to fix common problems on Debian-based distros -- they provide /usr/bin/qmllint,
            # which is a symlink to /usr/lib/x86_64-linux-gnu/qt4/bin/qmllint (or the Qt5 version of it).
            # The actual executable is part of different package, so might not even be installed =>
            # double-check whether qmllint is working by executing it
            execute_process(
                COMMAND ${QmlLint_EXECUTABLE} --version
                RESULT_VARIABLE _qmllint_result
                OUTPUT_QUIET ERROR_QUIET
            )
            if(_qmllint_result EQUAL 0)
                set(QMLLINT_IS_WORKING
                    TRUE
                    CACHE BOOL "Whether the found qmllint executable is actually usable" FORCE
                )
            endif()
        endif()
        if(QMLLINT_IS_WORKING)
            set(QmlLint_FOUND TRUE)
        endif()
    endif()
endif()

# validate a list of qml files
function(qml_lint)
    if(NOT QmlLint_EXECUTABLE OR NOT QmlLint_FOUND)
        return()
    endif()

    if(NOT TARGET qmllint_all)
        add_custom_target(qmllint_all ALL COMMENT "Scan all .qml files with qmllint")
    endif()

    foreach(_file ${ARGN})
        get_filename_component(_file_abs ${_file} ABSOLUTE)
        add_custom_command(
            OUTPUT ${_file}.lint
            COMMAND ${QmlLint_EXECUTABLE} ${_file_abs}
            COMMAND ${CMAKE_COMMAND} -E touch ${_file}.lint
            MAIN_DEPENDENCY ${_file_abs}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Run qmllint on ${_file}"
        )
        add_custom_target(
            ${_file}_qmllint
            DEPENDS ${_file}.lint
            COMMENT "Ensure qmllint is run on ${_file}"
        )
        add_dependencies(qmllint_all ${_file}_qmllint)
    endforeach()
endfunction()
