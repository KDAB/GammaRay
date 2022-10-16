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
# shared part
set(gammaray_statemachineviewer_shared_srcs statemachineviewerinterface.cpp)

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_statemachineviewer_plugin_srcs
        statemachineviewerserver.cpp
        transitionmodel.cpp
        statemodel.cpp
        statemachinewatcher.cpp
        statemachinedebuginterface.cpp
        qsmstatemachinedebuginterface.cpp
    )

    if(TARGET Qt::Scxml)
        list(APPEND gammaray_statemachineviewer_plugin_srcs qscxmlstatemachinedebuginterface.cpp)
    endif()

    gammaray_add_plugin(
        gammaray_statemachineviewer_plugin
        JSON
        gammaray_statemachineviewer.json
        SOURCES
        ${gammaray_statemachineviewer_plugin_srcs}
        ${gammaray_statemachineviewer_shared_srcs}
    )

    set_target_properties(gammaray_statemachineviewer_plugin PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)

    target_link_libraries(gammaray_statemachineviewer_plugin gammaray_core)

    if(TARGET Qt::Scxml)
        target_link_libraries(gammaray_statemachineviewer_plugin Qt::Scxml Qt::ScxmlPrivate)
    endif()
endif()

# UI part - dependencies
set_package_properties(
    KDSME PROPERTIES
    URL "https://github.com/KDAB/KDStateMachineEditor"
    DESCRIPTION "KDAB State Machine Editor framework"
    TYPE RECOMMENDED
    PURPOSE "Graphical state machine debugging."
)
find_package(KDSME 1.2 CONFIG QUIET)

# UI part
if(GAMMARAY_BUILD_UI AND KDSME_FOUND)

    set(gammaray_statemachineviewer_ui_plugin_srcs statemachineviewerwidget.cpp statemachineviewerclient.cpp
                                                   statemodeldelegate.cpp
    )

    gammaray_add_plugin(
        gammaray_statemachineviewer_ui_plugin
        JSON
        gammaray_statemachineviewer.json
        SOURCES
        ${gammaray_statemachineviewer_ui_plugin_srcs}
        ${gammaray_statemachineviewer_shared_srcs}
    )

    target_link_libraries(
        gammaray_statemachineviewer_ui_plugin
        KDSME::Core
        KDSME::View
        gammaray_common
        gammaray_ui
    )

endif()
