# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    set(gammaray_textdocumentinspector_srcs textdocumentinspector.cpp textdocumentformatmodel.cpp textdocumentmodel.cpp)
    gammaray_add_plugin(
        gammaray_textdocumentinspector
        JSON
        gammaray_textdocumentinspector.json
        SOURCES
        ${gammaray_textdocumentinspector_srcs}
    )
    target_link_libraries(gammaray_textdocumentinspector gammaray_core)
    target_link_libraries(gammaray_textdocumentinspector Qt::Gui)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_textdocumentinspector_ui_srcs textdocumentinspectorwidget.cpp textdocumentcontentview.cpp)

    gammaray_add_plugin(
        gammaray_textdocumentinspector_ui
        JSON
        gammaray_textdocumentinspector.json
        SOURCES
        ${gammaray_textdocumentinspector_ui_srcs}
    )
    target_link_libraries(gammaray_textdocumentinspector_ui gammaray_ui)
endif()
