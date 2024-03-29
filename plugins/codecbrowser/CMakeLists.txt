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
    set(gammaray_codecbrowser_plugin_srcs codecbrowser.cpp codecmodel.cpp)

    gammaray_add_plugin(
        gammaray_codecbrowser_plugin
        JSON
        gammaray_codecbrowser.json
        SOURCES
        ${gammaray_codecbrowser_plugin_srcs}
    )
    set_target_properties(gammaray_codecbrowser_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)

    target_link_libraries(gammaray_codecbrowser_plugin gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

    set(gammaray_codecbrowser_plugin_ui_srcs codecbrowserwidget.cpp)

    gammaray_add_plugin(
        gammaray_codecbrowser_ui_plugin
        JSON
        gammaray_codecbrowser.json
        SOURCES
        ${gammaray_codecbrowser_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_codecbrowser_ui_plugin gammaray_common gammaray_ui)

endif()
