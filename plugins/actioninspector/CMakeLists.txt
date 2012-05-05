include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_actioninspector_plugin_srcs
  ../../3rdparty/kde/kfilterproxysearchline.cpp
  ../../3rdparty/kde/krecursivefilterproxymodel.cpp

  actionvalidator.cpp
  actioninspector.cpp
  actionmodel.cpp
)

qt4_automoc(${gammaray_actioninspector_plugin_srcs})

gammaray_add_plugin(gammaray_actioninspector_plugin
  gammaray_actioninspector.desktop
  ${gammaray_actioninspector_plugin_srcs}
)

target_link_libraries(gammaray_actioninspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_probe
)
