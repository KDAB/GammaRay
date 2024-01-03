# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    if(TARGET Qt6::StateMachine)
        target_link_libraries(gammaray_statemachineviewer_plugin Qt6::StateMachine Qt6::StateMachinePrivate)
    endif()
endif()

# UI part - dependencies
set(KDSME_PACKAGE_NAME KDSME)
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    set(KDSME_PACKAGE_NAME KDSME-qt6)
endif()
set_package_properties(
    ${KDSME_PACKAGE_NAME} PROPERTIES
    URL "https://github.com/KDAB/KDStateMachineEditor"
    DESCRIPTION "KDAB State Machine Editor framework"
    TYPE RECOMMENDED
    PURPOSE "Graphical state machine debugging."
)
find_package(${KDSME_PACKAGE_NAME} 1.2 CONFIG QUIET)

# UI part
if(GAMMARAY_BUILD_UI AND ${KDSME_PACKAGE_NAME}_FOUND)

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

    if(TARGET Qt6::StateMachine)
        target_link_libraries(gammaray_statemachineviewer_ui_plugin Qt6::StateMachine)
    endif()
endif()
