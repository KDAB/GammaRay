# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    find_package(Wayland 1.12 COMPONENTS Server)
    set_package_properties(
        Wayland PROPERTIES
        PURPOSE "Needed for the QtWayland compositor inspector plug-in."
        TYPE OPTIONAL
    )

    if(TARGET Qt::WaylandCompositor AND Wayland_FOUND)
        set(gammaray_wlcompositorinspector_srcs
            clientsmodel.cpp
            clientsmodel.h
            resourceinfo.cpp
            resourceinfo.h
            wlcompositorinspector.cpp
            wlcompositorinspector.h
            wlcompositorinterface.cpp
            wlcompositorinterface.h
        )
        gammaray_add_plugin(
            gammaray_wlcompositorinspector
            JSON
            gammaray_wlcompositorinspector.json
            SOURCES
            ${gammaray_wlcompositorinspector_srcs}
        )
        target_link_libraries(
            gammaray_wlcompositorinspector
            gammaray_core
            gammaray_kitemmodels
            Qt::WaylandCompositor
            Qt::CorePrivate
            Wayland::Server
        )

    endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI AND (NOT MSVC OR NOT MSVC_VERSION LESS 1800)) # at least MSVC 2013

    set(gammaray_wlcompositorinspector_ui_srcs
        inspectorwidget.cpp
        inspectorwidget.h
        logview.cpp
        logview.h
        wlcompositorclient.cpp
        wlcompositorclient.h
        wlcompositorinterface.cpp
        wlcompositorinterface.h
    )

    gammaray_add_plugin(
        gammaray_wlcompositorinspector_ui
        JSON
        gammaray_wlcompositorinspector.json
        SOURCES
        ${gammaray_wlcompositorinspector_ui_srcs}
    )

    target_link_libraries(gammaray_wlcompositorinspector_ui Qt::Widgets gammaray_common gammaray_ui)

endif()
