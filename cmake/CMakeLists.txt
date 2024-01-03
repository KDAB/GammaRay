# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(FILES GammaRayMacros.cmake DESTINATION ${CMAKECONFIG_INSTALL_DIR})
endif()
