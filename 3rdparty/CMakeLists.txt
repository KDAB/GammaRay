# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2024 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

if(GAMMARAY_WITH_KDSME)
    function(add_ksme_subdirectory)
        # Function creates extra scope to keep these variables local
        set(KDSME_DOCS OFF)
        set(KDSME_EXAMPLES OFF)
        set(BUILD_TESTING OFF)

        set(KDSME_PACKAGE_NAME KDSME)
        if(QT_VERSION_MAJOR GREATER_EQUAL 6)
            set(KDSME_PACKAGE_NAME KDSME-qt6)
            set(KDSME_QT6 ON)
        else()
            set(KDSME_QT6 OFF)
        endif()
        set_package_properties(
            ${KDSME_PACKAGE_NAME} PROPERTIES
            URL "https://github.com/KDAB/KDStateMachineEditor"
            DESCRIPTION "KDAB State Machine Editor framework"
            TYPE RECOMMENDED
            PURPOSE "Graphical state machine debugging."
        )

        gammaray_ensure_submodule_exists(KDStateMachineEditor)
        # remove this as it clashes with Gammaray's DQT_DISABLE_DEPRECATED_BEFORE
        remove_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050500)
        add_subdirectory(KDStateMachineEditor)
    endfunction()
    add_ksme_subdirectory()
endif()

add_subdirectory(kde)
