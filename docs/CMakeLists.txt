# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
#
# find doc generation tools needed by multiple sub-directories
#

# qdoc toolchain
# TODO: some of this probably should be upstreamed to Qt's cmake files...
# TODO replace with Qt::qhelpgenerator for Qt >= 5.7.1
find_program(QHELPGEN_EXECUTABLE qhelpgenerator HINTS ${QT_INSTALL_BINS})
find_program(QDOC_EXECUTABLE qdoc HINTS ${QT_INSTALL_BINS})
find_program(QTATTRIBUTIONSSCANNER_EXECUTABLE qtattributionsscanner HINTS ${QT_INSTALL_BINS})

find_file(QDOC_TEMPLATE global/qt-html-templates-offline.qdocconf HINTS ${QT_INSTALL_DOCS} ${QT_INSTALL_DATA}
                                                                        ${QT_INSTALL_DATA}/doc
)
find_file(QDOC_QTCORE_INDEX qtcore.index HINTS ${QT_INSTALL_DOCS}/qtcore ${QT_INSTALL_DATA}/doc/qtcore)

if(NOT QDOC_EXECUTABLE
   OR NOT QHELPGEN_EXECUTABLE
   OR NOT QDOC_TEMPLATE
   OR NOT QDOC_QTCORE_INDEX
   OR NOT QTATTRIBUTIONSSCANNER_EXECUTABLE
)
    message(STATUS "Unable to build user manual in qch format.")
    message(STATUS "qdoc executable: ${QDOC_EXECUTABLE}")
    message(STATUS "qhelpgenerator executable: ${QHELPGEN_EXECUTABLE}")
    message(STATUS "qtattributionsscanner executable: ${QTATTRIBUTIONSSCANNER_EXECUTABLE}")
    message(STATUS "qdoc template (qt-html-templates-offline.qdocconf): ${QDOC_TEMPLATE}")
    message(STATUS "qdoc qtcore index (qtcore.index): ${QDOC_QTCORE_INDEX}")
    set(GAMMARAY_USER_MANUAL_BUILD False)
else()
    set(GAMMARAY_USER_MANUAL_BUILD True)
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
