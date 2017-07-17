# probe part
if(BUILD_TIMER_PLUGIN)

set(gammaray_timertop_plugin_srcs
  timertop.cpp
  timermodel.cpp
  timerinfo.cpp
  functioncalltimer.cpp
)

gammaray_add_plugin(gammaray_timertop_plugin
  DESKTOP gammaray_timertop.desktop.in
  JSON gammaray_timertop.json
  SOURCES ${gammaray_timertop_plugin_srcs}
)

target_link_libraries(gammaray_timertop_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

if (LINUX)
  target_link_libraries(pthread)
endif()

if(NOT WIN32 AND NOT APPLE)
  target_link_libraries(gammaray_timertop_plugin rt)
endif()

endif()

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_timertop_plugin_ui_srcs
    timertopwidget.cpp
  )

  gammaray_add_plugin(gammaray_timertop_ui_plugin
    DESKTOP gammaray_timertop_ui.desktop.in
    JSON gammaray_timertop.json
    SOURCES ${gammaray_timertop_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_timertop_ui_plugin
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )

endif()
