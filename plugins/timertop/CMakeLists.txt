if(BUILD_TIMER_PLUGIN)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_timertop_plugin_srcs
  timertop.cpp
  timermodel.cpp
  timerinfo.cpp
  functioncalltimer.cpp
)

qt4_automoc(${gammaray_timertop_plugin_srcs})

qt4_wrap_ui(gammaray_timertop_plugin_srcs
  timertop.ui
)

gammaray_add_plugin(gammaray_timertop_plugin
  gammaray_timertop.desktop
  ${gammaray_timertop_plugin_srcs}
)

target_link_libraries(gammaray_timertop_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_probe
)

if(NOT WIN32 AND NOT APPLE)
  target_link_libraries(gammaray_timertop_plugin rt)
endif()

endif()
