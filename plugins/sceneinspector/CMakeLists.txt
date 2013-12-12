include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_sceneinspector_plugin_srcs
  graphicsview.cpp
  graphicssceneview.cpp
  scenemodel.cpp
  sceneinspectorinterface.cpp
  sceneinspector.cpp
  sceneinspectorclient.cpp
  sceneinspectorwidget.cpp
)

qt4_wrap_ui(gammaray_sceneinspector_plugin_srcs
  graphicssceneview.ui
  sceneinspectorwidget.ui
)

gammaray_add_plugin(gammaray_sceneinspector_plugin
  gammaray_sceneinspector.desktop
  ${gammaray_sceneinspector_plugin_srcs}
)

target_link_libraries(gammaray_sceneinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
  gammaray_ui
)
