# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_webinspector_plugin_srcs webinspector.cpp webinspector.h webviewmodel.cpp webviewmodel.h)

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
    set(gammaray_webinspector_ui_plugin_srcs webinspectorwidget.cpp webinspectorwidget.h)

    gammaray_add_plugin(
        gammaray_webinspector_ui_plugin
        JSON
        gammaray_webinspector.json
        SOURCES
        ${gammaray_webinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_ui_plugin Qt::WebEngineWidgets gammaray_ui)
endif()
