# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    gammaray_add_plugin(
        gammaray_bluetooth
        JSON
        gammaray_bluetooth.json
        SOURCES
        bluetooth.cpp
        bluetooth.h
    )
    target_link_libraries(gammaray_bluetooth gammaray_core Qt::Bluetooth Qt::CorePrivate)
    set_target_properties(gammaray_bluetooth PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()
