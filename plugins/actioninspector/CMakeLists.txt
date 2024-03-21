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
    set(gammaray_actioninspector_plugin_srcs
        actioninspector.cpp
        actioninspector.h
        actionmodel.cpp
        actionmodel.h
        actionvalidator.cpp
        actionvalidator.h
    )

    gammaray_add_plugin(
        gammaray_actioninspector_plugin
        JSON
        gammaray_actioninspector.json
        SOURCES
        ${gammaray_actioninspector_plugin_srcs}
    )

    target_link_libraries(
        gammaray_actioninspector_plugin
        Qt::Gui
        Qt::CorePrivate
        Qt::Widgets
        gammaray_core
    )
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_actioninspector_ui_plugin_srcs actioninspectorwidget.cpp actioninspectorwidget.h clientactionmodel.cpp
                                                clientactionmodel.h
    )

    gammaray_add_plugin(
        gammaray_actioninspector_ui_plugin
        JSON
        gammaray_actioninspector.json
        SOURCES
        ${gammaray_actioninspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_actioninspector_ui_plugin gammaray_ui)
endif()
