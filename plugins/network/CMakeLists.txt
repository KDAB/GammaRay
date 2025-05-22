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
    set(gammaray_network_srcs
        cookies/cookieextension.cpp
        cookies/cookieextension.h
        cookies/cookiejarmodel.cpp
        cookies/cookiejarmodel.h
        networkinterfacemodel.cpp
        networkinterfacemodel.h
        networkreplymodel.cpp
        networkreplymodel.h
        networksupport.cpp
        networksupport.h
        networksupportinterface.cpp
        networksupportinterface.h
    )

    gammaray_add_plugin(
        gammaray_network
        JSON
        gammaray_network.json
        SOURCES
        ${gammaray_network_srcs}
    )
    target_link_libraries(gammaray_network gammaray_core Qt::Network Qt::CorePrivate)
    set_target_properties(gammaray_network PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_network_ui_srcs
        clientnetworkconfigurationmodel.cpp
        clientnetworkconfigurationmodel.h
        clientnetworkreplymodel.cpp
        clientnetworkreplymodel.h
        cookies/cookietab.cpp
        cookies/cookietab.h
        networkconfigurationwidget.cpp
        networkconfigurationwidget.h
        networkinterfacewidget.cpp
        networkinterfacewidget.h
        networkreplywidget.cpp
        networkreplywidget.h
        networksupportclient.cpp
        networksupportclient.h
        networksupportinterface.cpp
        networksupportinterface.h
        networkwidget.cpp
        networkwidget.h
    )
    gammaray_add_plugin(
        gammaray_network_ui
        JSON
        gammaray_network.json
        SOURCES
        ${gammaray_network_ui_srcs}
    )
    target_link_libraries(gammaray_network_ui gammaray_ui Qt::Network)
endif()
