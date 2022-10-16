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
# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    find_package(Wayland 1.12 COMPONENTS Server)
    set_package_properties(
        Wayland PROPERTIES
        PURPOSE "Needed for the QtWayland compositor inspector plug-in."
        TYPE OPTIONAL
    )

    if(TARGET Qt::WaylandCompositor AND Wayland_FOUND)
        set(gammaray_wlcompositorinspector_srcs wlcompositorinspector.cpp wlcompositorinterface.cpp clientsmodel.cpp
                                                resourceinfo.cpp
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
            Wayland::Server
        )

    endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI AND (NOT MSVC OR NOT MSVC_VERSION LESS 1800)) # at least MSVC 2013

    set(gammaray_wlcompositorinspector_ui_srcs inspectorwidget.cpp wlcompositorinterface.cpp wlcompositorclient.cpp
                                               logview.cpp
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
