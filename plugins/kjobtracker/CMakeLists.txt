# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

find_package(KF${QtCore_VERSION_MAJOR}CoreAddons NO_MODULE QUIET)
set_package_properties(
    KF${QtCore_VERSION_MAJOR}CoreAddons PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "KDE KCoreAddons Framework"
    URL "https://www.kde.org/"
    PURPOSE "Required for the KJob tracker plug-in."
)

# probe part
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    if(TARGET KF${QtCore_VERSION_MAJOR}::CoreAddons)
        set(gammaray_kjob_plugin_srcs kjobmodel.cpp kjobmodel.h kjobtracker.cpp kjobtracker.h)

        gammaray_add_plugin(
            gammaray_kjobtracker_plugin
            JSON
            gammaray_kjobtracker.json
            SOURCES
            ${gammaray_kjob_plugin_srcs}
        )

        target_link_libraries(gammaray_kjobtracker_plugin gammaray_core KF${QtCore_VERSION_MAJOR}::CoreAddons Qt::Gui)
    endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_kjob_ui_plugin_srcs kjobtrackerwidget.cpp kjobtrackerwidget.h)

    gammaray_add_plugin(
        gammaray_kjobtracker_ui_plugin
        JSON
        gammaray_kjobtracker.json
        SOURCES
        ${gammaray_kjob_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_kjobtracker_ui_plugin gammaray_ui)
endif()
