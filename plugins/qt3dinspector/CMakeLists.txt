# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        EXPORT
        GammaRayTargets
    )
    target_link_libraries(
        gammaray_3dinspector
        PUBLIC gammaray_core
        PRIVATE gammaray_kitemmodels
    )
    target_link_libraries(gammaray_3dinspector PRIVATE Qt::3DRender Qt::3DInput)
    if(TARGET Qt::3DAnimation)
        target_link_libraries(gammaray_3dinspector PRIVATE Qt::3DAnimation)
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
    target_link_libraries(gammaray_3dinspector_ui PUBLIC gammaray_ui)
    target_link_libraries(gammaray_3dinspector_ui PRIVATE Qt::3DRender Qt::3DInput Qt::3DLogic Qt::3DExtras)
endif()
