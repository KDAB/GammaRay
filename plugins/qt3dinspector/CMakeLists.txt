add_subdirectory(geometryextension)

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

  ${gammaray_3dinspector_shared_srcs}
)
gammaray_add_plugin(gammaray_3dinspector JSON gammaray_3dinspector.json SOURCES ${gammaray_3dinspector_srcs})
target_link_libraries(gammaray_3dinspector gammaray_core gammaray_kitemmodels Qt5::3DInput Qt5::3DRender)
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
  target_link_libraries(gammaray_3dinspector_ui gammaray_ui Qt5::3DRender Qt5::3DInput Qt5::3DLogic Qt5::3DExtras)
endif()
