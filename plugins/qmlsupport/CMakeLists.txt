# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_qmlsupport_srcs
        qmlsupport.cpp
        qmllistpropertyadaptor.cpp
        qmlattachedpropertyadaptor.cpp
        qjsvaluepropertyadaptor.cpp
        qmlcontextpropertyadaptor.cpp
        qmlcontextextension.cpp
        qmlcontextmodel.cpp
        qmltypeextension.cpp
    )

    gammaray_add_plugin(
        gammaray_qmlsupport
        JSON
        gammaray_qmlsupport.json
        SOURCES
        ${gammaray_qmlsupport_srcs}
    )
    target_link_libraries(
        gammaray_qmlsupport
        gammaray_core
        Qt::Gui
        Qt::Qml
        Qt::QmlPrivate
    )
endif()

if(NOT QtQml_VERSION VERSION_LESS 5.10 OR TARGET Qt6::Qml)
    target_sources(gammaray_qmlsupport PUBLIC qmlbindingprovider.cpp)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)

    set(gammaray_qmlsupport_ui_srcs qmlsupportuifactory.cpp qmlcontexttab.cpp qmltypetab.cpp)
    gammaray_add_plugin(
        gammaray_qmlsupport_ui
        JSON
        gammaray_qmlsupport.json
        SOURCES
        ${gammaray_qmlsupport_ui_srcs}
    )
    target_link_libraries(gammaray_qmlsupport_ui gammaray_ui)
endif()
