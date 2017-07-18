if(Qt5Quick_FOUND)
  # shared stuff
  set(gammaray_quickinspector_shared_srcs
        quickinspectorinterface.cpp
        quickitemgeometry.cpp
        quickdecorationsdrawer.cpp
        materialextension/materialextensioninterface.cpp
      )

  add_library(gammaray_quickinspector_shared STATIC ${gammaray_quickinspector_shared_srcs})
  target_link_libraries(gammaray_quickinspector_shared gammaray_common Qt5::Gui Qt5::Quick)

  set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

  set(gammaray_quickinspector_srcs
    quickinspector.cpp
    quickitemmodel.cpp
    quickscenegraphmodel.cpp
    quickpaintanalyzerextension.cpp
    quickoverlay.cpp
    materialextension/materialextension.cpp
    materialextension/materialshadermodel.cpp
    materialextension/qquickopenglshadereffectmaterialadaptor.cpp
    geometryextension/sggeometryextension.cpp
    geometryextension/sggeometrymodel.cpp
    textureextension/textureextension.cpp
    textureextension/qsgtexturegrabber.cpp
  )

  gammaray_add_plugin(gammaray_quickinspector
    JSON gammaray_quickinspector.json
    SOURCES ${gammaray_quickinspector_srcs}
  )

  target_include_directories(gammaray_quickinspector SYSTEM PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
  target_link_libraries(gammaray_quickinspector
    gammaray_quickinspector_shared
    gammaray_core Qt5::Quick
    gammaray_kitemmodels
  )

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
      geometryextension/sggeometrytab.cpp
      geometryextension/sgwireframewidget.cpp
      textureextension/texturetab.cpp
      textureextension/textureviewwidget.cpp
    )

    gammaray_add_plugin(gammaray_quickinspector_ui
      JSON gammaray_quickinspector.json
      SOURCES ${gammaray_quickinspector_ui_srcs}
    )

    target_link_libraries(gammaray_quickinspector_ui
      gammaray_quickinspector_shared
      gammaray_ui
      Qt5::Quick
    )
  endif()
endif()
