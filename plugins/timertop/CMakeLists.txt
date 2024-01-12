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
    set(gammaray_timertop_plugin_srcs
        timerinfo.cpp
        timerinfo.h
        timermodel.cpp
        timermodel.h
        timertop.cpp
        timertop.h
        timertopinterface.cpp
        timertopinterface.h
    )

    gammaray_add_plugin(
        gammaray_timertop_plugin
        JSON
        gammaray_timertop.json
        SOURCES
        ${gammaray_timertop_plugin_srcs}
    )
    set_target_properties(gammaray_timertop_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)

    target_link_libraries(gammaray_timertop_plugin gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

    set(gammaray_timertop_plugin_ui_srcs
        clienttimermodel.cpp
        clienttimermodel.h
        timertopclient.cpp
        timertopclient.h
        timertopinterface.cpp
        timertopinterface.h
        timertopwidget.cpp
        timertopwidget.h
    )

    gammaray_add_plugin(
        gammaray_timertop_ui_plugin
        JSON
        gammaray_timertop.json
        SOURCES
        ${gammaray_timertop_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_timertop_ui_plugin gammaray_ui)

endif()
