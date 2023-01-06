# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#
if(TARGET Qt::Widgets)
    set(gammaray_injector_style_srcs injectorstyleplugin.cpp)

    add_library(gammaray_injector_style MODULE ${gammaray_injector_style_srcs})

    target_link_libraries(
        gammaray_injector_style
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::GuiPrivate
    )

    set_target_properties(
        gammaray_injector_style PROPERTIES PREFIX "" LIBRARY_OUTPUT_DIRECTORY
                                                     ${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}/styles
    )
    gammaray_set_rpath(gammaray_injector_style ${PROBE_PLUGIN_INSTALL_DIR}/styles)
    target_compile_features(gammaray_injector_style PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

    if(GAMMARAY_INSTALL_QT_LAYOUT)
        set_target_properties(
            gammaray_injector_style PROPERTIES OUTPUT_NAME gammaray_injector_style-${GAMMARAY_PROBE_ABI}
        )
    endif()

    install(TARGETS gammaray_injector_style DESTINATION ${PROBE_PLUGIN_INSTALL_DIR}/styles)
    if(MSVC)
        install(
            FILES "$<TARGET_PDB_FILE_DIR:gammaray_injector_style>/$<TARGET_PDB_FILE_NAME:gammaray_injector_style>"
            DESTINATION "${PROBE_PLUGIN_INSTALL_DIR}/styles"
            CONFIGURATIONS Debug RelWithDebInfo
        )
    endif()
endif()
