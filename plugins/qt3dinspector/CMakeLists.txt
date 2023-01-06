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

# shared stuff
set(gammaray_3dinspector_shared_srcs qt3dinspectorinterface.cpp geometryextension/qt3dgeometryextensioninterface.cpp)

# probe plugin
if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_3dinspector_srcs
        3dinspector.cpp
        qt3dentitytreemodel.cpp
        framegraphmodel.cpp
        geometryextension/qt3dgeometryextension.cpp
        paintanalyzerextension/qt3dpaintedtextureanalyzerextension.cpp
        ${gammaray_3dinspector_shared_srcs}
    )
    gammaray_add_plugin(
        gammaray_3dinspector
        JSON
        gammaray_3dinspector.json
        SOURCES
        ${gammaray_3dinspector_srcs}
    )
    target_link_libraries(
        gammaray_3dinspector
        gammaray_core
        gammaray_kitemmodels
        Qt::3DInput
        Qt::3DRender
    )
    if(TARGET Qt::3DAnimation)
        target_link_libraries(gammaray_3dinspector Qt::3DAnimation)
    endif()
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)

    set(gammaray_3dinspector_ui_srcs
        qt3dinspectorwidget.cpp
        qt3dinspectorclient.cpp
        treeexpander.cpp
        geometryextension/qt3dgeometryextensionclient.cpp
        geometryextension/qt3dgeometrytab.cpp
        geometryextension/attribute.cpp
        geometryextension/boundingvolume.cpp
        geometryextension/buffermodel.cpp
        geometryextension/cameracontroller.cpp
        ${gammaray_3dinspector_shared_srcs}
        qt3dinspector.qrc
    )

    gammaray_add_plugin(
        gammaray_3dinspector_ui
        JSON
        gammaray_3dinspector.json
        SOURCES
        ${gammaray_3dinspector_ui_srcs}
    )
    target_link_libraries(
        gammaray_3dinspector_ui
        gammaray_ui
        Qt::3DRender
        Qt::3DInput
        Qt::3DLogic
        Qt::3DExtras
    )
endif()
