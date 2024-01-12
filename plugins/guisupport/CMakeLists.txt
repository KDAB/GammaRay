# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_guisupport_srcs guisupport.cpp guisupport.h ui.qrc)
    gammaray_add_plugin(
        gammaray_guisupport
        JSON
        gammaray_guisupport.json
        SOURCES
        ${gammaray_guisupport_srcs}
    )
    target_link_libraries(gammaray_guisupport gammaray_core Qt::Gui Qt::GuiPrivate)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_guisupport_ui_srcs guisupportuifactory.cpp guisupportuifactory.h paintanalyzertab.cpp
                                    paintanalyzertab.h
    )
    gammaray_add_plugin(
        gammaray_guisupport_ui
        JSON
        gammaray_guisupport.json
        SOURCES
        ${gammaray_guisupport_ui_srcs}
    )
    target_link_libraries(gammaray_guisupport_ui gammaray_ui)
endif()
