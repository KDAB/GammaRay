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
if(UNIX)
    find_program(POD2MAN_EXECUTABLE pod2man)
    gammaray_add_dummy_package(pod2man POD2MAN_EXECUTABLE)
    set_package_properties(
        pod2man PROPERTIES
        TYPE RECOMMENDED
        DESCRIPTION "Man page generator"
        PURPOSE "Generate GammaRay man pages."
    )

    if(POD2MAN_EXECUTABLE)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
            COMMAND ${POD2MAN_EXECUTABLE} -c "KDAB" -r "\"${GAMMARAY_VERSION}\"" -s 1
                    ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod
            COMMENT "Generate the GammaRay man page"
        )
        add_custom_target(
            man ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
            COMMENT "Target to generate the man pages"
        )

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1 DESTINATION ${MAN_INSTALL_DIR})
    endif()
endif()
