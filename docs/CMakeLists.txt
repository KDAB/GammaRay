# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# qdoc toolchain
find_package(Qt${QT_VERSION_MAJOR} NO_MODULE QUIET OPTIONAL_COMPONENTS Help)

find_package(Qt${QT_VERSION_MAJOR} NO_MODULE QUIET OPTIONAL_COMPONENTS ToolsTools)

if(TARGET Qt::qdoc)
    set(QDOC_EXECUTABLE "$<TARGET_FILE:Qt::qdoc>")
else()
    find_program(QDOC_EXECUTABLE qdoc HINTS ${QT_INSTALL_BINS})
endif()

if(TARGET Qt::qhelpgenerator)
    # Required for Doxygen
    get_target_property(QHELPGEN_EXECUTABLE Qt::qhelpgenerator IMPORTED_LOCATION)
else()
    find_program(QHELPGEN_EXECUTABLE qhelpgenerator HINTS ${QT_INSTALL_BINS} ${QT_INSTALL_LIBEXECS})
endif()

if(TARGET Qt::qtattributionsscanner)
    set(QTATTRIBUTIONSSCANNER_EXECUTABLE Qt::qtattributionsscanner)
else()
    find_program(QTATTRIBUTIONSSCANNER_EXECUTABLE qtattributionsscanner HINTS ${QT_INSTALL_BINS} ${QT_INSTALL_LIBEXECS})
endif()

if(NOT DEFINED QDOC_TEMPLATE_DIR)
    # compute the qdoc template dir from where qt-html-templates-offline.qdocconf is found
    find_file(QDOC_TEMPLATE global/qt-html-templates-offline.qdocconf HINTS ${QT_INSTALL_DOCS} ${QT_INSTALL_DATA}
                                                                            ${QT_INSTALL_DATA}/doc
    )
    if(QDOC_TEMPLATE)
        get_filename_component(QDOC_TEMPLATE_DIR ${QDOC_TEMPLATE} DIRECTORY)
        get_filename_component(QDOC_TEMPLATE_DIR ${QDOC_TEMPLATE_DIR} DIRECTORY)
    endif()
endif()

if(NOT DEFINED QDOC_INDEX_DIR)
    # try to compute the qdoc index dir from where qtcore.index is found
    find_file(QDOC_INDEX qtcore.index HINTS ${QT_INSTALL_DOCS}/qtcore ${QT_INSTALL_DATA}/doc/qtcore)
    if(QDOC_INDEX)
        get_filename_component(QDOC_INDEX_DIR ${QDOC_INDEX} DIRECTORY)
        get_filename_component(QDOC_INDEX_DIR ${QDOC_INDEX_DIR} DIRECTORY)
    endif()
endif()

if(NOT QDOC_EXECUTABLE
   OR NOT QHELPGEN_EXECUTABLE
   OR NOT QDOC_TEMPLATE_DIR
   OR NOT QTATTRIBUTIONSSCANNER_EXECUTABLE
)
    # Expand imported target data for status reporting
    if(TARGET Qt::qdoc)
        get_target_property(QDOC_EXECUTABLE Qt::qdoc IMPORTED_LOCATION)
    endif()
    if(TARGET Qt::qtattributionsscanner)
        get_target_property(QTATTRIBUTIONSSCANNER_EXECUTABLE Qt::qtattributionsscanner IMPORTED_LOCATION)
    endif()
    foreach(_var QDOC_INDEX QDOC_TEMPLATE)
        if(NOT DEFINED ${_var})
            set(${_var} "${_var}_DIR=${${_var}_DIR}")
        endif()
    endforeach()

    message(STATUS "Unable to build user manual in qch format.")
    message(STATUS "qdoc executable: ${QDOC_EXECUTABLE}")
    message(STATUS "qhelpgenerator executable: ${QHELPGEN_EXECUTABLE}")
    message(STATUS "qtattributionsscanner executable: ${QTATTRIBUTIONSSCANNER_EXECUTABLE}")
    message(STATUS "qdoc template: ${QDOC_TEMPLATE}")
    message(STATUS "qdoc index: ${QDOC_INDEX}")
    set(GAMMARAY_USER_MANUAL_BUILD False)
else()
    set(GAMMARAY_USER_MANUAL_BUILD True)
    if(NOT QDOC_INDEX_DIR)
        message(STATUS "Couldn't find index dir, manual won't link to system documentation")
    endif()
endif()

add_feature_info(
    "User Manual creation in qch format" GAMMARAY_USER_MANUAL_BUILD
    "Requires qdoc, qhelpgenerator, qtattributionsscanner, the qdoc templates and index files"
)

# Doxygen
find_package(Doxygen)
set_package_properties(
    Doxygen PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "API Documentation system"
    URL "https://www.doxygen.org"
    PURPOSE "Needed to build the API documentation."
)
if(DOXYGEN_DOT_EXECUTABLE)
    set(HAVE_DOT "YES")
else()
    set(HAVE_DOT "NO")
    message(STATUS "Unable to provide inheritance diagrams for the API documentation. "
                   "To fix, install the graphviz project from https://www.graphviz.org"
    )
endif()

if(DOXYGEN_FOUND AND QHELPGEN_EXECUTABLE)
    set(GAMMARAY_API_DOCS_BUILD True)
else()
    set(GAMMARAY_API_DOCS_BUILD False)
endif()

#
# generate docs
#

if(GAMMARAY_API_DOCS_BUILD)
    add_subdirectory(api)
endif()
if(GAMMARAY_USER_MANUAL_BUILD)
    add_subdirectory(manual)
    add_subdirectory(collection)
endif()
add_subdirectory(man)
