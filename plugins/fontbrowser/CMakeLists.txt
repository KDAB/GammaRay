include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_fontbrowser_plugin_srcs
  fontbrowserinterface.cpp
  fontbrowser.cpp
  fontbrowserwidget.cpp
  fontmodel.cpp
  fontbrowserclient.cpp
  fontbrowserserver.cpp
)

qt4_automoc(${gammaray_fontbrowser_plugin_srcs})
qt4_wrap_ui(gammaray_fontbrowser_plugin_srcs
  fontbrowserwidget.ui
)

gammaray_add_plugin(gammaray_fontbrowser_plugin
  gammaray_fontbrowser.desktop
  ${gammaray_fontbrowser_plugin_srcs}
)

target_link_libraries(gammaray_fontbrowser_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
  gammaray_ui
)
