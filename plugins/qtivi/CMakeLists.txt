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
    if(TARGET Qt::IviCore)
        set(plugin_files qtivisupport.cpp)
        if(QtIviCore_VERSION VERSION_GREATER 1.1)
            add_definitions(-DQTIVI_DISABLE_IVIPROPERTIES)
            set(plugin_files ${plugin_files} qtiviobjectmodel.cpp)
        else()
            set(plugin_files ${plugin_files} qtivipropertymodel.cpp qtivipropertyoverrider.cpp)
        endif()
        gammaray_add_plugin(
            gammaray_qtivi
            JSON
            gammaray_qtivi.json
            SOURCES
            ${plugin_files}
        )
        target_link_libraries(gammaray_qtivi gammaray_core Qt::IviCore Qt::IviCorePrivate)
        if(TARGET Qt::IviVehicleFunctions)
            target_link_libraries(gammaray_qtivi Qt::IviVehicleFunctions)
        endif()
        if(TARGET QtIviMedia)
            target_link_libraries(gammaray_qtivi Qt::IviMedia)
        endif()
    endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    gammaray_add_plugin(
        gammaray_qtivi_ui
        JSON
        gammaray_qtivi.json
        SOURCES
        qtivisupportwidget.cpp
        qtiviconstrainedvaluedelegate.cpp
        qtivipropertyclientmodel.cpp
    )
    target_link_libraries(gammaray_qtivi_ui gammaray_ui)
endif()
