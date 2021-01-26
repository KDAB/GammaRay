if(TARGET Qt::Quick)
  # shared stuff
  set(gammaray_quickinspector_shared_srcs
        quickinspectorinterface.cpp
        quickitemgeometry.cpp
        quickdecorationsdrawer.cpp
        materialextension/materialextensioninterface.cpp
      )

  add_library(gammaray_quickinspector_shared STATIC ${gammaray_quickinspector_shared_srcs})
  target_link_libraries(gammaray_quickinspector_shared gammaray_common Qt::Gui Qt::Quick)

  set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_compile_features(gammaray_quickinspector_shared PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

  if(NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_quickinspector_srcs
      quickinspector.cpp

      quickanchorspropertyadaptor.cpp
      quickitemmodel.cpp
      quickscenegraphmodel.cpp
      quickpaintanalyzerextension.cpp
      quickscreengrabber.cpp
    )

    if(NOT QT_NO_OPENGL)
      list(APPEND gammaray_quickinspector_srcs
        materialextension/materialextension.cpp
        materialextension/materialshadermodel.cpp
        materialextension/qquickopenglshadereffectmaterialadaptor.cpp
        geometryextension/sggeometryextension.cpp
        geometryextension/sggeometrymodel.cpp
        textureextension/textureextension.cpp
        textureextension/qsgtexturegrabber.cpp)
    endif()

    gammaray_add_plugin(gammaray_quickinspector
      JSON gammaray_quickinspector.json
      SOURCES ${gammaray_quickinspector_srcs}
    )

    if(NOT Qt5Quick_VERSION VERSION_LESS 5.7)
      target_sources(gammaray_quickinspector PUBLIC ${CMAKE_CURRENT_LIST_DIR}/quickimplicitbindingdependencyprovider.cpp)
    endif()

    target_link_libraries(gammaray_quickinspector
      gammaray_quickinspector_shared
      gammaray_core
      Qt::Quick
      Qt::QuickPrivate
      gammaray_kitemmodels
    )
  endif()

  if(TARGET Qt::Gui_GLESv2)
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} Qt::Gui_GLESv2)
    check_cxx_source_compiles("int main() {return 0;}" CAN_LINK_GUI_GLESV2)
    if(CAN_LINK_GUI_GLESV2)
      set(HAVE_GLESv2 TRUE)
      target_link_libraries(gammaray_quickinspector Qt::Gui_GLESv2)
    endif()
  endif()
  #the Qt::Gui_GLESv2 target is not set on Android (up to qt5.13, at least)
  if(ANDROID AND NOT TARGET Qt::Gui_GLESv2)
    target_link_libraries(gammaray_quickinspector GLESv2)
  endif()

  if(GAMMARAY_BUILD_UI)
    # ui plugin
    set(gammaray_quickinspector_ui_srcs
      quickinspectorwidget.cpp
      quickinspectorclient.cpp
      quickclientitemmodel.cpp
      quickitemdelegate.cpp
      quickitemtreewatcher.cpp
      quickscenepreviewwidget.cpp
      quickscenecontrolwidget.cpp
      quickoverlaylegend.cpp
      gridsettingswidget.cpp

      materialextension/materialextensionclient.cpp
      materialextension/materialtab.cpp
      textureextension/texturetab.cpp
      textureextension/textureviewwidget.cpp
      textureextension/resources.qrc
    )

    if(NOT QT_NO_OPENGL)
      list(APPEND gammaray_quickinspector_ui_srcs
        geometryextension/sggeometrytab.cpp
        geometryextension/sgwireframewidget.cpp
      )
    endif()

    gammaray_add_plugin(gammaray_quickinspector_ui
      JSON gammaray_quickinspector.json
      SOURCES ${gammaray_quickinspector_ui_srcs}
    )

    target_link_libraries(gammaray_quickinspector_ui
      gammaray_quickinspector_shared
      gammaray_ui
      Qt::Quick
    )
  endif()
endif()
