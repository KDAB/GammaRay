# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_sceneinspector_plugin_srcs sceneinspector.cpp scenemodel.cpp sceneinspectorinterface.cpp
                                            paintanalyzerextension.cpp
    )

    gammaray_add_plugin(
        gammaray_sceneinspector_plugin
        JSON
        gammaray_sceneinspector.json
        SOURCES
        ${gammaray_sceneinspector_plugin_srcs}
    )

    target_link_libraries(
        gammaray_sceneinspector_plugin
        Qt::Gui
        Qt::Widgets
        gammaray_kitemmodels
        gammaray_core
    )
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_sceneinspector_ui_plugin_srcs
        sceneinspectorwidget.cpp
        graphicsview.cpp
        graphicssceneview.cpp
        sceneinspectorinterface.cpp
        sceneinspectorclient.cpp
    )

    gammaray_add_plugin(
        gammaray_sceneinspector_ui_plugin
        JSON
        gammaray_sceneinspector.json
        SOURCES
        ${gammaray_sceneinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_sceneinspector_ui_plugin gammaray_ui)
endif()
