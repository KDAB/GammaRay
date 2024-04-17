# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

find_package(Qt${QT_VERSION_MAJOR} NO_MODULE QUIET OPTIONAL_COMPONENTS Help)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} NO_MODULE QUIET OPTIONAL_COMPONENTS ToolsTools)
endif()
if(NOT TARGET Qt::qhelpgenerator)
    message(STATUS "qhelpgenerator not found, documentation collection will not be generated.")
    return()
endif()

set(manual_qch_file ${CMAKE_CURRENT_BINARY_DIR}/../manual/gammaray-manual/gammaray-manual.qch)
set(manual_qch_target gammaray-manual.qch)
if(GAMMARAY_API_DOCS_BUILD)
    set(apidocs_qch_file ${CMAKE_CURRENT_BINARY_DIR}/../api/qch/gammaray-api.qch)
    set(apidocs_qch_target gammaray-api.qch)
    set(API_DOCS_QCH_FILE_TAG "<file>gammaray-api.qch</file>")
else()
    set(apidocs_qch_file ${manual_qch_file}) # trick to make the copy command valid below
    set(apidocs_qch_target ${manual_qch_target})
    set(API_DOCS_QCH_FILE_TAG "")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gammaray.qhcp.cmake ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhcp)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    COMMAND ${CMAKE_COMMAND} -E copy ${manual_qch_file} ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${apidocs_qch_file} ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND Qt::qhelpgenerator ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhcp -o ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.qhcp.cmake ${manual_qch_target} ${apidocs_qch_target}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate the GammaRay .qhc file using qhelpgenerator"
)
add_custom_target(
    gammaray_qhc ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    COMMENT "Target to generate the GammaRay .qhc file"
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    DESTINATION ${QCH_INSTALL_DIR}
)
