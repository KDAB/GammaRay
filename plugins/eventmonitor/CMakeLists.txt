# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

    set(gammaray_eventmonitor_plugin_srcs
        eventmonitor.cpp
        eventmodel.cpp
        eventmonitorinterface.cpp
        eventtypemodel.cpp
        eventtypefilter.cpp
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
    set(gammaray_eventmonitor_ui_plugin_srcs eventmonitorwidget.cpp eventmonitorinterface.cpp eventmonitorclient.cpp
                                             eventtypeclientproxymodel.cpp
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
