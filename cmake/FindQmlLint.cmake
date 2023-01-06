# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

find_program(QMLLINT_EXECUTABLE qmllint)
if(QMLLINT_EXECUTABLE)
    if(NOT QMLLINT_IS_WORKING)
        # Try to fix common problems on Debian-based distros -- they provide /usr/bin/qmllint,
        # which is a symlink to /usr/lib/x86_64-linux-gnu/qt4/bin/qmllint (or the Qt5 version of it).
        # The actual executable is part of different package, so might not even be installed =>
        # double-check whether qmllint is working by executing it
        execute_process(
            COMMAND ${QMLLINT_EXECUTABLE} --version
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

# validate a list of qml files
function(qml_lint)
    if(NOT QMLLINT_EXECUTABLE OR NOT QmlLint_FOUND)
        return()
    endif()

    foreach(_file ${ARGN})
        get_filename_component(_file_abs ${_file} ABSOLUTE)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint
            COMMAND ${QMLLINT_EXECUTABLE} ${_file_abs}
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint
            MAIN_DEPENDENCY ${_file_abs}
            COMMENT "Run qmlint on the specified files and create the associated timestamp semaphore"
        )
        add_custom_target(
            ${_file}_qmllint ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint
            COMMENT "Target to run qmllint on the specified file"
        )
    endforeach()
endfunction()
