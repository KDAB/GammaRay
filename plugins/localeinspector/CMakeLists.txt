# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#
# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_localeinspector_plugin_srcs localeinspector.cpp localemodel.cpp localedataaccessor.cpp
                                             localeaccessormodel.cpp
    )
    list(APPEND gammaray_localeinspector_plugin_srcs timezonemodel.cpp timezoneoffsetdatamodel.cpp)

    gammaray_add_plugin(
        gammaray_localeinspector_plugin
        JSON
        gammaray_localeinspector.json
        SOURCES
        ${gammaray_localeinspector_plugin_srcs}
    )
    set_target_properties(gammaray_localeinspector_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)

    target_link_libraries(gammaray_localeinspector_plugin gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_localeinspector_plugin_ui_srcs
        localeinspectorwidget.cpp
        localetab.cpp
        timezonetab.cpp
        timezoneclientmodel.cpp
        timezoneoffsetdataclientmodel.cpp
    )

    gammaray_add_plugin(
        gammaray_localeinspector_ui_plugin
        JSON
        gammaray_localeinspector.json
        SOURCES
        ${gammaray_localeinspector_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_localeinspector_ui_plugin gammaray_ui)
endif()
