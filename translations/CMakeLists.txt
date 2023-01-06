# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
set(LANGUAGES en de)

# translation tools
add_executable(
    ljsonupdate
    ljsonupdate.cpp
)
target_link_libraries(
    ljsonupdate Qt::Core
)

# ts file update (manual target, affects source dir)
file(GLOB_RECURSE jsons "${CMAKE_SOURCE_DIR}/plugins/*.json")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp
    COMMAND ljsonupdate --extract --out ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp ${jsons}
    DEPENDS ${jsons}
    COMMENT "Run this project's ljsonupdate to extract the translations from the plugin json files"
)

add_custom_target(ts COMMENT "Target to extract all translations")
foreach(lang ${LANGUAGES})
    add_custom_target(
        ts_${lang}
        COMMAND
            Qt::lupdate 3rdparty app client common core inprocessui launcher plugins ui
            ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp -locations relative -no-ui-lines -no-obsolete -ts
            ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${lang}.ts
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp
        COMMENT "Run lupdate to extract translation from Qt UI files, C++ etc"
    )
    add_dependencies(ts ts_${lang})
endforeach()

# qm generation and installation
foreach(lang ${LANGUAGES})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm
        COMMAND Qt::lrelease ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${lang}.ts -qm
                ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${lang}.ts
        COMMENT "Run lrelease to generate the translations for the specified language"
    )
    add_custom_target(
        gammaray_${lang}_qm ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm
        COMMENT "Target to generate the translations for specified language"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm DESTINATION ${TRANSLATION_INSTALL_DIR})
endforeach()

# message merging (manual target, affects source dir)
add_custom_target(tsmerge COMMENT "Target to merge all translations")
foreach(lang ${LANGUAGES})
    add_custom_target(
        tsmerge_${lang}
        COMMAND ljsonupdate --merge --qm ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm --lang ${lang} ${jsons}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${lang}.qm
        COMMENT "Run this project's ljsonupdate to merge the translations for the specified language"
    )
    add_dependencies(tsmerge tsmerge_${lang})
endforeach()
