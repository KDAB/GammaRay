# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_subdirectory(core)
    add_subdirectory(cli)
    if(HAVE_QT_CONCURRENT AND GAMMARAY_BUILD_UI)
        add_subdirectory(ui)
        add_subdirectory(app)
    endif()
endif()

if(GAMMARAY_BUILD_CLI_INJECTOR)
    add_subdirectory(win-injector)
endif()
