# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_timertop_plugin_srcs
  timertop.cpp
  timertopinterface.cpp
  timermodel.cpp
  timerinfo.cpp
)

gammaray_add_plugin(gammaray_timertop_plugin
  DESKTOP gammaray_timertop.desktop.in
  JSON gammaray_timertop.json
  SOURCES ${gammaray_timertop_plugin_srcs}
)

target_link_libraries(gammaray_timertop_plugin
  gammaray_core
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_timertop_plugin_ui_srcs
    timertopwidget.cpp
    timertopinterface.cpp
    timertopclient.cpp
    clienttimermodel.cpp
  )

  gammaray_add_plugin(gammaray_timertop_ui_plugin
    DESKTOP gammaray_timertop_ui.desktop.in
    JSON gammaray_timertop.json
    SOURCES ${gammaray_timertop_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_timertop_ui_plugin
    gammaray_ui
  )

endif()
