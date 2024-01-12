# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# shared part
set(gammaray_signalmonitor_shared_srcs signalmonitorcommon.cpp signalmonitorcommon.h signalmonitorinterface.cpp
                                       signalmonitorinterface.h
)
add_library(
    gammaray_signalmonitor_shared STATIC
    ${gammaray_signalmonitor_shared_srcs}
)
target_link_libraries(
    gammaray_signalmonitor_shared
    PRIVATE gammaray_common
)
set_target_properties(gammaray_signalmonitor_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_signalmonitor_shared PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_signalmonitor_srcs
        relativeclock.cpp
        relativeclock.h
        signalhistorymodel.cpp
        signalhistorymodel.h
        signalmonitor.cpp
        signalmonitor.h
    )

    gammaray_add_plugin(
        gammaray_signalmonitor
        JSON
        gammaray_signalmonitor.json
        SOURCES
        ${gammaray_signalmonitor_srcs}
    )

    target_link_libraries(gammaray_signalmonitor gammaray_core gammaray_signalmonitor_shared)
    target_link_libraries(gammaray_signalmonitor Qt::Gui)
endif()

if(GAMMARAY_BUILD_UI)
    # ui plugin
    set(gammaray_signalmonitor_ui_srcs
        signalhistorydelegate.cpp
        signalhistorydelegate.h
        signalhistoryview.cpp
        signalhistoryview.h
        signalmonitorclient.cpp
        signalmonitorclient.h
        signalmonitorwidget.cpp
        signalmonitorwidget.h
    )

    gammaray_add_plugin(
        gammaray_signalmonitor_ui
        JSON
        gammaray_signalmonitor.json
        SOURCES
        ${gammaray_signalmonitor_ui_srcs}
    )

    target_link_libraries(gammaray_signalmonitor_ui gammaray_ui gammaray_signalmonitor_shared)
endif()
