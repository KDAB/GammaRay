# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    gammaray_add_plugin(
        gammaray_openglsupport
        JSON
        gammaray_openglsupport.json
        SOURCES
        openglsupport.cpp
        openglsupport.h
    )
    target_link_libraries(gammaray_openglsupport gammaray_core Qt::Gui Qt::CorePrivate)
    if(${QT_VERSION_MAJOR} EQUAL 6 AND TARGET Qt6::OpenGL)
        target_link_libraries(gammaray_openglsupport Qt6::OpenGL)
    endif()
endif()
