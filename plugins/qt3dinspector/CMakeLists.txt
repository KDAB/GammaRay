# shared stuff
set(gammaray_3dinspector_shared_srcs
  qt3dinspectorinterface.cpp
)

# probe plugin
set(gammaray_3dinspector_srcs
  ${gammaray_3dinspector_shared_srcs}
  3dinspector.cpp
  qt3dentitytreemodel.cpp
  framegraphmodel.cpp
)
gammaray_add_plugin(gammaray_3dinspector JSON gammaray_3dinspector.json SOURCES ${gammaray_3dinspector_srcs})
target_link_libraries(gammaray_3dinspector gammaray_core Qt5::3DRender)

# ui plugin
if(GAMMARAY_BUILD_UI)

  set(gammaray_3dinspector_ui_srcs
    ${gammaray_3dinspector_shared_srcs}
    qt3dinspectorwidget.cpp
    qt3dinspectorclient.cpp
  )
  qt4_wrap_ui(gammaray_3dinspector_ui_srcs
    qt3dinspectorwidget.ui
  )
  gammaray_add_plugin(gammaray_3dinspector_ui JSON gammaray_3dinspector.json SOURCES ${gammaray_3dinspector_ui_srcs})
  target_link_libraries(gammaray_3dinspector_ui gammaray_ui)
endif()
