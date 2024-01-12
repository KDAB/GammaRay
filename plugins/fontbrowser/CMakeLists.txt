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
    set(gammaray_fontbrowser_plugin_srcs
        fontbrowser.cpp
        fontbrowser.h
        fontbrowserinterface.cpp
        fontbrowserinterface.h
        fontbrowserserver.cpp
        fontbrowserserver.h
        fontdatabasemodel.cpp
        fontdatabasemodel.h
        fontmodel.cpp
        fontmodel.h
    )

    gammaray_add_plugin(
        gammaray_fontbrowser_plugin
        JSON
        gammaray_fontbrowser.json
        SOURCES
        ${gammaray_fontbrowser_plugin_srcs}
    )

    target_link_libraries(gammaray_fontbrowser_plugin gammaray_core)
    target_link_libraries(gammaray_fontbrowser_plugin Qt::Gui)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

    set(gammaray_fontbrowser_plugin_ui_srcs
        fontbrowserclient.cpp
        fontbrowserclient.h
        fontbrowserinterface.cpp
        fontbrowserinterface.h
        fontbrowserwidget.cpp
        fontbrowserwidget.h
    )

    gammaray_add_plugin(
        gammaray_fontbrowser_ui_plugin
        JSON
        gammaray_fontbrowser.json
        SOURCES
        ${gammaray_fontbrowser_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_fontbrowser_ui_plugin gammaray_common gammaray_ui)

endif()
