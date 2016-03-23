# probe plugin
set(gammaray_wlcompositorinspector_srcs
  wlcompositorinspector.cpp
  wlcompositorinterface.cpp
)
gammaray_add_plugin(gammaray_wlcompositorinspector
  JSON gammaray_wlcompositorinspector.json SOURCES
  ${gammaray_wlcompositorinspector_srcs}
)
target_link_libraries(gammaray_wlcompositorinspector
  gammaray_core gammaray_kitemmodels Qt5::WaylandCompositor wayland-server)

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_wlcompositorinspector_ui_srcs
    inspectorwidget.cpp
    wlcompositorinterface.cpp
    wlcompositorclient.cpp
    logview.cpp
  )

  qt5_wrap_ui(gammaray_wlcompositorinspector_ui_srcs
    inspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_wlcompositorinspector_ui
    JSON gammaray_wlcompositorinspector.json
    SOURCES ${gammaray_wlcompositorinspector_ui_srcs}
  )


  target_link_libraries(gammaray_wlcompositorinspector_ui
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_common
    gammaray_ui
  )

endif()
