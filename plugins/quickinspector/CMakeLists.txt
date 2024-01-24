# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

if(NOT TARGET Qt::Quick)
    # We can't do anything here
    return()
endif()

# shared stuff
set(gammaray_quickinspector_shared_srcs quickinspectorinterface.cpp quickitemgeometry.cpp quickdecorationsdrawer.cpp
                                        materialextension/materialextensioninterface.cpp
)

add_library(
    gammaray_quickinspector_shared OBJECT
    ${gammaray_quickinspector_shared_srcs}
)
target_link_libraries(
    gammaray_quickinspector_shared
    PUBLIC gammaray_common
)
target_link_libraries(
    gammaray_quickinspector_shared
    PUBLIC Qt::Gui Qt::Quick
)

set_target_properties(
    gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE $<IF:$<BOOL:GAMMARAY_STATIC_PROBE>,OFF,ON>
)
target_compile_features(gammaray_quickinspector_shared PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_quickinspector_srcs
        quickanchorspropertyadaptor.cpp
        quickanchorspropertyadaptor.h
        quickinspector.cpp
        quickinspector.h
        quickitemmodel.cpp
        quickitemmodel.h
        quickpaintanalyzerextension.cpp
        quickpaintanalyzerextension.h
        quickscenegraphmodel.cpp
        quickscenegraphmodel.h
        quickscreengrabber.cpp
        quickscreengrabber.h
    )

    if(NOT QT_NO_OPENGL)
        list(
            APPEND
            gammaray_quickinspector_srcs
            geometryextension/sggeometryextension.cpp
            geometryextension/sggeometryextension.h
            geometryextension/sggeometrymodel.cpp
            geometryextension/sggeometrymodel.h
            materialextension/materialextension.cpp
            materialextension/materialextension.h
            materialextension/materialshadermodel.cpp
            materialextension/materialshadermodel.h
            materialextension/qquickopenglshadereffectmaterialadaptor.cpp
            materialextension/qquickopenglshadereffectmaterialadaptor.h
            textureextension/qsgtexturegrabber.cpp
            textureextension/qsgtexturegrabber.h
            textureextension/textureextension.cpp
            textureextension/textureextension.h
        )
    endif()

    gammaray_add_plugin(
        gammaray_quickinspector
        JSON
        gammaray_quickinspector.json
        SOURCES
        ${gammaray_quickinspector_srcs}
        EXPORT
        GammaRayTargets
    )

    if(TARGET Qt${QT_MAJOR_VERSION}::Quick)
        target_sources(
            gammaray_quickinspector PRIVATE ${CMAKE_CURRENT_LIST_DIR}/quickimplicitbindingdependencyprovider.cpp
        )
    endif()

    target_link_libraries(
        gammaray_quickinspector
        PUBLIC gammaray_core $<BUILD_INTERFACE:gammaray_quickinspector_shared>
        PRIVATE gammaray_kitemmodels
    )
    target_link_libraries(
        gammaray_quickinspector
        PUBLIC Qt::Quick
        PRIVATE Qt::QuickPrivate
    )

    if(TARGET Qt::Gui_GLESv2)
        include(CheckCXXSourceCompiles)
        list(APPEND CMAKE_REQUIRED_LIBRARIES Qt::Gui_GLESv2)
        check_cxx_source_compiles("int main() {return 0;}" CAN_LINK_GUI_GLESV2)
        if(CAN_LINK_GUI_GLESV2)
            set(HAVE_GLESv2 TRUE)
            target_link_libraries(gammaray_quickinspector PRIVATE Qt::Gui_GLESv2)
        endif()
    endif()

    #the Qt::Gui_GLESv2 target is not set on Android (up to qt5.13, at least)
    if(ANDROID AND NOT TARGET Qt::Gui_GLESv2)
        target_link_libraries(gammaray_quickinspector PRIVATE GLESv2)
    endif()
endif()

if(GAMMARAY_BUILD_UI)
    # ui plugin
    set(gammaray_quickinspector_ui_srcs
        gridsettingswidget.cpp
        gridsettingswidget.h
        materialextension/materialextensionclient.cpp
        materialextension/materialextensionclient.h
        materialextension/materialtab.cpp
        materialextension/materialtab.h
        quickclientitemmodel.cpp
        quickclientitemmodel.h
        quickinspectorclient.cpp
        quickinspectorclient.h
        quickinspectorwidget.cpp
        quickinspectorwidget.h
        quickitemdelegate.cpp
        quickitemdelegate.h
        quickitemtreewatcher.cpp
        quickitemtreewatcher.h
        quickoverlaylegend.cpp
        quickoverlaylegend.h
        quickscenecontrolwidget.cpp
        quickscenecontrolwidget.h
        quickscenepreviewwidget.cpp
        quickscenepreviewwidget.h
        textureextension/texturetab.cpp
        textureextension/texturetab.h
        textureextension/textureviewwidget.cpp
        textureextension/textureviewwidget.h
        textureextension/resources.qrc
    )

    if(NOT QT_NO_OPENGL)
        list(
            APPEND
            gammaray_quickinspector_ui_srcs
            geometryextension/sggeometrytab.cpp
            geometryextension/sggeometrytab.h
            geometryextension/sgwireframewidget.cpp
            geometryextension/sgwireframewidget.h
        )
    endif()

    gammaray_add_plugin(
        gammaray_quickinspector_ui
        JSON
        gammaray_quickinspector.json
        SOURCES
        ${gammaray_quickinspector_ui_srcs}
    )

    target_link_libraries(
        gammaray_quickinspector_ui
        PUBLIC gammaray_ui
        PRIVATE $<BUILD_INTERFACE:gammaray_quickinspector_shared>
    )
    target_link_libraries(gammaray_quickinspector_ui PRIVATE Qt::Quick)

endif()
