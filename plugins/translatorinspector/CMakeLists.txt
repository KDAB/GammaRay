# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    gammaray_add_plugin(
        gammaray_translatorinspector
        JSON
        gammaray_translatorinspector.json
        SOURCES
        translatorinspector.cpp
        translatorinspectorinterface.cpp
        translatorwrapper.cpp
        translatorsmodel.cpp
    )
    target_link_libraries(gammaray_translatorinspector gammaray_core Qt::Core Qt::CorePrivate)
    set_target_properties(gammaray_translatorinspector PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    gammaray_add_plugin(
        gammaray_translatorinspector_ui
        JSON
        gammaray_translatorinspector.json
        SOURCES
        translatorinspectorwidget.cpp
        translatorinspectorinterface.cpp
        translatorwrapperproxy.cpp
    )
    target_link_libraries(gammaray_translatorinspector_ui gammaray_ui)
endif()
