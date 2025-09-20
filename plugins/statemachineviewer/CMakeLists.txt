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
        qsmstatemachinedebuginterface.cpp
        qsmstatemachinedebuginterface.h
        statemachinedebuginterface.cpp
        statemachinedebuginterface.h
        statemachineviewerserver.cpp
        statemachineviewerserver.h
        statemachinewatcher.cpp
        statemachinewatcher.h
        statemodel.cpp
        statemodel.h
        transitionmodel.cpp
        transitionmodel.h
    )

    if(TARGET Qt::Scxml)
        list(APPEND gammaray_statemachineviewer_plugin_srcs qscxmlstatemachinedebuginterface.cpp
             qscxmlstatemachinedebuginterface.h
        )
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
        if(Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
            find_package(Qt6ScxmlPrivate REQUIRED NO_MODULE)
        endif()
        target_link_libraries(gammaray_statemachineviewer_plugin Qt::Scxml Qt::ScxmlPrivate)
    endif()
    if(TARGET Qt6::StateMachine)
        if(Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
            find_package(Qt6StateMachinePrivate REQUIRED NO_MODULE)
        endif()
        target_link_libraries(gammaray_statemachineviewer_plugin Qt6::StateMachine Qt6::StateMachinePrivate)
    endif()
endif()

# UI part
if(GAMMARAY_BUILD_UI AND GAMMARAY_WITH_KDSME)

    set(gammaray_statemachineviewer_ui_plugin_srcs
        statemachineviewerclient.cpp
        statemachineviewerclient.h
        statemachineviewerwidget.cpp
        statemachineviewerwidget.h
        statemodeldelegate.cpp
        statemodeldelegate.h
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

    if(${QT_VERSION_MAJOR} EQUAL 6 AND TARGET Qt6::StateMachine)
        target_link_libraries(gammaray_statemachineviewer_ui_plugin Qt6::StateMachine)
    endif()
endif()
