# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_quickwidgetsupport_srcs quickwidgetsupport.cpp quickwidgetsupport.h)
    gammaray_add_plugin(
        gammaray_quickwidgetsupport
        JSON
        gammaray_quickwidgetsupport.json
        SOURCES
        ${gammaray_quickwidgetsupport_srcs}
    )
    target_link_libraries(gammaray_quickwidgetsupport gammaray_core Qt::QuickWidgets Qt::CorePrivate)
endif()
