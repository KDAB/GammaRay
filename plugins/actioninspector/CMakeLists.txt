include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_actioninspector_plugin_srcs
  actionvalidator.cpp
  actioninspector.cpp
  actioninspectorwidget.cpp
  actionmodel.cpp
)

gammaray_add_plugin(gammaray_actioninspector_plugin
  gammaray_actioninspector.desktop
  ${gammaray_actioninspector_plugin_srcs}
)

target_link_libraries(gammaray_actioninspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
  gammaray_ui
)
