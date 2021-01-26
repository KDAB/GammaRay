add_subdirectory(geometryextension/es2)
add_subdirectory(geometryextension/gl3)

# shared stuff
set(gammaray_3dinspector_shared_srcs
  qt3dinspectorinterface.cpp
  geometryextension/qt3dgeometryextensioninterface.cpp
)

# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_3dinspector_srcs
  3dinspector.cpp
  qt3dentitytreemodel.cpp
  framegraphmodel.cpp

  geometryextension/qt3dgeometryextension.cpp
  paintanalyzerextension/qt3dpaintedtextureanalyzerextension.cpp

  ${gammaray_3dinspector_shared_srcs}
)
gammaray_add_plugin(gammaray_3dinspector JSON gammaray_3dinspector.json SOURCES ${gammaray_3dinspector_srcs})
target_link_libraries(gammaray_3dinspector gammaray_core gammaray_kitemmodels Qt::3DInput Qt::3DRender)
if (TARGET Qt::3DAnimation)
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

  gammaray_add_plugin(gammaray_3dinspector_ui JSON gammaray_3dinspector.json SOURCES ${gammaray_3dinspector_ui_srcs})
  target_link_libraries(gammaray_3dinspector_ui gammaray_ui Qt::3DRender Qt::3DInput Qt::3DLogic Qt::3DExtras)
endif()
