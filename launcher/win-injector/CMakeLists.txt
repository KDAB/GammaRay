# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
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
