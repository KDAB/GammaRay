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
  JSON gammaray_fontbrowser.json
  SOURCES ${gammaray_fontbrowser_plugin_srcs}
)

target_link_libraries(gammaray_fontbrowser_plugin gammaray_core)
target_link_libraries(gammaray_fontbrowser_plugin Qt::Gui)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_fontbrowser_plugin_ui_srcs
    fontbrowserwidget.cpp
    fontbrowserinterface.cpp
    fontbrowserclient.cpp
  )

  gammaray_add_plugin(gammaray_fontbrowser_ui_plugin
    JSON gammaray_fontbrowser.json
    SOURCES ${gammaray_fontbrowser_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_fontbrowser_ui_plugin
    gammaray_common
    gammaray_ui
  )

endif()
