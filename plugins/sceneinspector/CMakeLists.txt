include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/ui
)

set(gammaray_sceneinspector_plugin_srcs
#   ../../3rdparty/kde/kfilterproxysearchline.cpp
#   ../../3rdparty/kde/krecursivefilterproxymodel.cpp

  graphicsview.cpp
  graphicssceneview.cpp
  scenemodel.cpp
  sceneinspector.cpp
)

qt4_automoc(${gammaray_sceneinspector_plugin_srcs})
qt4_wrap_ui(gammaray_sceneinspector_plugin_srcs
  graphicssceneview.ui
  sceneinspector.ui
)

gammaray_add_plugin(gammaray_sceneinspector_plugin
  gammaray_sceneinspector.desktop
  ${gammaray_sceneinspector_plugin_srcs}
)

target_link_libraries(gammaray_sceneinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_probe
)
