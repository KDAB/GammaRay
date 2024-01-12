# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    gammaray_add_plugin(
        gammaray_translatorinspector
        JSON
        gammaray_translatorinspector.json
        SOURCES
        translatorinspector.cpp
        translatorinspector.h
        translatorinspectorinterface.cpp
        translatorinspectorinterface.h
        translatorsmodel.cpp
        translatorsmodel.h
        translatorwrapper.cpp
        translatorwrapper.h
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
        translatorinspectorinterface.cpp
        translatorinspectorinterface.h
        translatorinspectorwidget.cpp
        translatorinspectorwidget.h
        translatorwrapperproxy.cpp
        translatorwrapperproxy.h
    )
    target_link_libraries(gammaray_translatorinspector_ui gammaray_ui)
endif()
