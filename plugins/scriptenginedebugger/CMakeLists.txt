# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_scriptenginedebugger_plugin_srcs scriptenginedebugger.cpp)

    gammaray_add_plugin(
        gammaray_scriptenginedebugger_plugin
        JSON
        gammaray_scriptenginedebugger.json
        SOURCES
        ${gammaray_scriptenginedebugger_plugin_srcs}
    )

    target_link_libraries(gammaray_scriptenginedebugger_plugin Qt5::Script Qt5::ScriptTools gammaray_core)
    set_target_properties(gammaray_scriptenginedebugger_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# UI plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_scriptenginedebugger_ui_plugin_srcs scriptenginedebuggerwidget.cpp)

    gammaray_add_plugin(
        gammaray_scriptenginedebugger_ui_plugin
        JSON
        gammaray_scriptenginedebugger.json
        SOURCES
        ${gammaray_scriptenginedebugger_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_scriptenginedebugger_ui_plugin Qt5::Script Qt5::ScriptTools gammaray_ui)
endif()
