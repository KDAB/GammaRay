# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    set(gammaray_webinspector_plugin_srcs webinspector.cpp webviewmodel.cpp)

    gammaray_add_plugin(
        gammaray_webinspector_plugin
        JSON
        gammaray_webinspector.json
        SOURCES
        ${gammaray_webinspector_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_plugin gammaray_core)

    set_target_properties(gammaray_webinspector_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# ui part
if(GAMMARAY_BUILD_UI AND TARGET Qt::WebEngineWidgets)
    set(gammaray_webinspector_ui_plugin_srcs webinspectorwidget.cpp)

    gammaray_add_plugin(
        gammaray_webinspector_ui_plugin
        JSON
        gammaray_webinspector.json
        SOURCES
        ${gammaray_webinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_ui_plugin Qt::WebEngineWidgets gammaray_ui)
endif()
