# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_fontbrowser_plugin_srcs
  fontbrowser.cpp
  fontbrowserinterface.cpp
  fontmodel.cpp
  fontdatabasemodel.cpp
  fontbrowserserver.cpp
)

gammaray_add_plugin(gammaray_fontbrowser_plugin
  DESKTOP gammaray_fontbrowser.desktop.in
  JSON gammaray_fontbrowser.json
  SOURCES ${gammaray_fontbrowser_plugin_srcs}
)

target_link_libraries(gammaray_fontbrowser_plugin gammaray_core)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_fontbrowser_plugin Qt5::Gui)
else()
  target_link_libraries(gammaray_fontbrowser_plugin ${QT_QTGUI_LIBRARIES})
endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_fontbrowser_plugin_ui_srcs
    fontbrowserwidget.cpp
    fontbrowserinterface.cpp
    fontbrowserclient.cpp
  )

  gammaray_add_plugin(gammaray_fontbrowser_ui_plugin
    DESKTOP gammaray_fontbrowser_ui.desktop.in
    JSON gammaray_fontbrowser.json
    SOURCES ${gammaray_fontbrowser_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_fontbrowser_ui_plugin
    gammaray_common
    gammaray_ui
  )

endif()
