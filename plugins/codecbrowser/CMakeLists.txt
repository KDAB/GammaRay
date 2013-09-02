include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_codecbrowser_plugin_srcs
  codecbrowser.cpp
  codecbrowserwidget.cpp
  codecmodel.cpp
)

qt4_automoc(${gammaray_codecbrowser_plugin_srcs})
qt4_wrap_ui(gammaray_codecbrowser_plugin_srcs
  codecbrowserwidget.ui
)

gammaray_add_plugin(gammaray_codecbrowser_plugin
  gammaray_codecbrowser.desktop
  ${gammaray_codecbrowser_plugin_srcs}
)

target_link_libraries(gammaray_codecbrowser_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
)
