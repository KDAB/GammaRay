# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
    gammaray_add_plugin(
        gammaray_bluetooth
        JSON
        gammaray_bluetooth.json
        SOURCES
        bluetooth.cpp
    )
    target_link_libraries(gammaray_bluetooth gammaray_core Qt::Bluetooth)
    set_target_properties(gammaray_bluetooth PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()
