# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_models_srcs
        modelinspector.cpp
        modelinspectorinterface.cpp
        modelmodel.cpp
        modelcellmodel.cpp
        modelcontentproxymodel.cpp
        selectionmodelmodel.cpp
        modelinspector.h
        modelinspectorinterface.h
        modelmodel.h
        modelcellmodel.h
        modelcontentproxymodel.h
        selectionmodelmodel.h
    )

    gammaray_add_plugin(
        gammaray_modelinspector
        JSON
        gammaray_modelinspector.json
        SOURCES
        ${gammaray_models_srcs}
    )
    set_target_properties(gammaray_modelinspector PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)

    target_link_libraries(gammaray_modelinspector gammaray_core gammaray_kitemmodels)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

    set(gammaray_models_ui_srcs
        modelcontentdelegate.cpp
        modelcontentdelegate.h
        modelinspectorclient.cpp
        modelinspectorclient.h
        modelinspectorinterface.cpp
        modelinspectorinterface.h
        modelinspectorwidget.cpp
        modelinspectorwidget.h
    )

    gammaray_add_plugin(
        gammaray_modelinspector_ui
        JSON
        gammaray_modelinspector.json
        SOURCES
        ${gammaray_models_ui_srcs}
    )

    target_link_libraries(gammaray_modelinspector_ui gammaray_ui)

endif()
