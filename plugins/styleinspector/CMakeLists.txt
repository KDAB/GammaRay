# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    set(gammaray_styleinspector_plugin_srcs
        styleinspector.cpp
        pixelmetricmodel.cpp
        standardiconmodel.cpp
        primitivemodel.cpp
        controlmodel.cpp
        styleoption.cpp
        abstractstyleelementmodel.cpp
        abstractstyleelementstatetable.cpp
        complexcontrolmodel.cpp
        dynamicproxystyle.cpp
        styleinspectorinterface.cpp
        stylehintmodel.cpp
        ${CMAKE_SOURCE_DIR}/ui/palettemodel.cpp
    )

    gammaray_add_plugin(
        gammaray_styleinspector_plugin
        JSON
        gammaray_styleinspector.json
        SOURCES
        ${gammaray_styleinspector_plugin_srcs}
    )

    target_link_libraries(gammaray_styleinspector_plugin Qt::Gui Qt::Widgets gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_styleinspector_ui_plugin_srcs styleinspectorwidget.cpp styleelementstatetablepage.cpp
                                               styleinspectorinterface.cpp styleinspectorclient.cpp
    )

    gammaray_add_plugin(
        gammaray_styleinspector_ui_plugin
        JSON
        gammaray_styleinspector.json
        SOURCES
        ${gammaray_styleinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_styleinspector_ui_plugin gammaray_ui)
endif()
