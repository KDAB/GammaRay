include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_selectionmodelinspector_plugin_srcs
  selectionmodelinspector.cpp
  selectionmodelinspectorwidget.cpp
)

qt4_automoc(${gammaray_selectionmodelinspector_plugin_srcs})
qt4_wrap_ui(gammaray_selectionmodelinspector_plugin_srcs
  selectionmodelinspectorwidget.ui
)

gammaray_add_plugin(gammaray_selectionmodelinspector_plugin
  gammaray_selectionmodelinspector.desktop
  ${gammaray_selectionmodelinspector_plugin_srcs}
)

target_link_libraries(gammaray_selectionmodelinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
)
