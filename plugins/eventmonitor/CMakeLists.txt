# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)

    set(gammaray_eventmonitor_plugin_srcs
        eventmodel.cpp
        eventmodel.h
        eventmonitor.cpp
        eventmonitor.h
        eventmonitorinterface.cpp
        eventmonitorinterface.h
        eventtypefilter.cpp
        eventtypefilter.h
        eventtypemodel.cpp
        eventtypemodel.h
    )

    gammaray_add_plugin(
        gammaray_eventmonitor_plugin
        JSON
        gammaray_eventmonitor.json
        SOURCES
        ${gammaray_eventmonitor_plugin_srcs}
    )

    target_link_libraries(gammaray_eventmonitor_plugin Qt::Gui Qt::CorePrivate gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_eventmonitor_ui_plugin_srcs
        eventmonitorclient.cpp
        eventmonitorclient.h
        eventmonitorinterface.cpp
        eventmonitorinterface.h
        eventmonitorwidget.cpp
        eventmonitorwidget.h
        eventtypeclientproxymodel.cpp
        eventtypeclientproxymodel.h
    )

    gammaray_add_plugin(
        gammaray_eventmonitor_ui_plugin
        JSON
        gammaray_eventmonitor.json
        SOURCES
        ${gammaray_eventmonitor_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_eventmonitor_ui_plugin gammaray_ui)
endif()
