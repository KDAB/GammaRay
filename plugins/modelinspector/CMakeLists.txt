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
# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_models_srcs
        modelinspector.cpp
        modelinspectorinterface.cpp
        modelmodel.cpp
        modelcellmodel.cpp
        modelcontentproxymodel.cpp
        selectionmodelmodel.cpp
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

    set(gammaray_models_ui_srcs modelinspectorwidget.cpp modelinspectorinterface.cpp modelinspectorclient.cpp
                                modelcontentdelegate.cpp
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
