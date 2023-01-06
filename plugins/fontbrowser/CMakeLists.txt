# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    set(gammaray_fontbrowser_plugin_srcs
        fontbrowser.cpp
        fontbrowserinterface.cpp
        fontmodel.cpp
        fontdatabasemodel.cpp
        fontbrowserserver.cpp
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

    set(gammaray_fontbrowser_plugin_ui_srcs fontbrowserwidget.cpp fontbrowserinterface.cpp fontbrowserclient.cpp)

    gammaray_add_plugin(
        gammaray_fontbrowser_ui_plugin
        JSON
        gammaray_fontbrowser.json
        SOURCES
        ${gammaray_fontbrowser_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_fontbrowser_ui_plugin gammaray_common gammaray_ui)

endif()
