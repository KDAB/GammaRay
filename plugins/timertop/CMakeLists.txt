# probe part
if(BUILD_TIMER_PLUGIN)

set(gammaray_timertop_plugin_srcs
  timertop.cpp
  timermodel.cpp
  timerinfo.cpp
  functioncalltimer.cpp
)

gammaray_add_plugin(gammaray_timertop_plugin
  gammaray_timertop.desktop
  ${gammaray_timertop_plugin_srcs}
)

target_link_libraries(gammaray_timertop_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

if(NOT WIN32 AND NOT APPLE)
  target_link_libraries(gammaray_timertop_plugin rt)
endif()

endif()

# ui part
if((Qt5Widgets_FOUND OR QT_QTGUI_FOUND) AND NOT GAMMARAY_PROBE_ONLY_BUILD)

  set(gammaray_timertop_plugin_ui_srcs
    timertopwidget.cpp
  )

  qt4_wrap_ui(gammaray_timertop_plugin_ui_srcs
    timertopwidget.ui
  )

  gammaray_add_plugin(gammaray_timertop_ui_plugin
    gammaray_timertop_ui.desktop
    ${gammaray_timertop_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_timertop_ui_plugin
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_common
  )

endif()
