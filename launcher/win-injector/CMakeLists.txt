# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
if(WIN32)
    add_executable(
        gammaray_wininjector ${CMAKE_CURRENT_SOURCE_DIR}/../core/injector/basicwindllinjector.cpp wininjector-cli.cpp
    )
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_target_properties(gammaray_wininjector PROPERTIES OUTPUT_NAME "gammaray-wininjector-x86_64")
    else()
        set_target_properties(gammaray_wininjector PROPERTIES OUTPUT_NAME "gammaray-wininjector-i686")
    endif()
    install(TARGETS gammaray_wininjector ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
