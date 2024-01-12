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
    set(gammaray_styleinspector_plugin_srcs
        ${CMAKE_SOURCE_DIR}/ui/palettemodel.cpp
        ${CMAKE_SOURCE_DIR}/ui/palettemodel.h
        abstractstyleelementmodel.cpp
        abstractstyleelementmodel.h
        abstractstyleelementstatetable.cpp
        abstractstyleelementstatetable.h
        complexcontrolmodel.cpp
        complexcontrolmodel.h
        controlmodel.cpp
        controlmodel.h
        dynamicproxystyle.cpp
        dynamicproxystyle.h
        pixelmetricmodel.cpp
        pixelmetricmodel.h
        primitivemodel.cpp
        primitivemodel.h
        standardiconmodel.cpp
        standardiconmodel.h
        stylehintmodel.cpp
        stylehintmodel.h
        styleinspector.cpp
        styleinspector.h
        styleinspectorinterface.cpp
        styleinspectorinterface.h
        styleoption.cpp
        styleoption.h
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
    set(gammaray_styleinspector_ui_plugin_srcs
        styleelementstatetablepage.cpp
        styleelementstatetablepage.h
        styleinspectorclient.cpp
        styleinspectorclient.h
        styleinspectorinterface.cpp
        styleinspectorinterface.h
        styleinspectorwidget.cpp
        styleinspectorwidget.h
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
