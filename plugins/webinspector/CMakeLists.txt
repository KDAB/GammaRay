include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_webinspector_plugin_srcs
  webinspector.cpp
  webinspectorwidget.cpp
  webviewmodel.cpp
)

qt4_wrap_ui(gammaray_webinspector_plugin_srcs webinspectorwidget.ui)

gammaray_add_plugin(gammaray_webinspector_plugin
  gammaray_webinspector.desktop
  ${gammaray_webinspector_plugin_srcs}
)

target_link_libraries(gammaray_webinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTWEBKIT_LIBRARIES}
  ${Qt5WebKitWidgets_LIBRARIES}
  gammaray_core
)
