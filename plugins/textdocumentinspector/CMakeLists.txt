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
    set(gammaray_textdocumentinspector_srcs
        textdocumentformatmodel.cpp
        textdocumentformatmodel.h
        textdocumentinspector.cpp
        textdocumentinspector.h
        textdocumentmodel.cpp
        textdocumentmodel.h
    )
    gammaray_add_plugin(
        gammaray_textdocumentinspector
        JSON
        gammaray_textdocumentinspector.json
        SOURCES
        ${gammaray_textdocumentinspector_srcs}
    )
    target_link_libraries(gammaray_textdocumentinspector gammaray_core)
    target_link_libraries(gammaray_textdocumentinspector Qt::Gui Qt::CorePrivate)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_textdocumentinspector_ui_srcs textdocumentcontentview.cpp textdocumentcontentview.h
                                               textdocumentinspectorwidget.cpp textdocumentinspectorwidget.h
    )

    gammaray_add_plugin(
        gammaray_textdocumentinspector_ui
        JSON
        gammaray_textdocumentinspector.json
        SOURCES
        ${gammaray_textdocumentinspector_ui_srcs}
    )
    target_link_libraries(gammaray_textdocumentinspector_ui gammaray_ui)
endif()
