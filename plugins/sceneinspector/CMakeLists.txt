# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_sceneinspector_plugin_srcs
        paintanalyzerextension.cpp
        paintanalyzerextension.h
        sceneinspector.cpp
        sceneinspector.h
        sceneinspectorinterface.cpp
        sceneinspectorinterface.h
        scenemodel.cpp
        scenemodel.h
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
        Qt::CorePrivate
        Qt::Widgets
        gammaray_kitemmodels
        gammaray_core
    )
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_sceneinspector_ui_plugin_srcs
        graphicssceneview.cpp
        graphicssceneview.h
        graphicsview.cpp
        graphicsview.h
        sceneinspectorclient.cpp
        sceneinspectorclient.h
        sceneinspectorinterface.cpp
        sceneinspectorinterface.h
        sceneinspectorwidget.cpp
        sceneinspectorwidget.h
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
