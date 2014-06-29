# probe plugin
set(gammaray_signalmonitor_srcs
  signalmonitor.cpp
  signalhistorymodel.cpp
  relativeclock.cpp
)

gammaray_add_plugin(gammaray_signalmonitor
  gammaray_signalmonitor.desktop
  ${gammaray_signalmonitor_srcs}
)

target_link_libraries(gammaray_signalmonitor gammaray_core)
if (Qt5_FOUND)
  target_link_libraries(gammaray_signalmonitor Qt5::Gui)
endif ()

# ui plugin
set(gammaray_signalmonitor_ui_srcs
  signalhistorydelegate.cpp
  signalhistoryview.cpp
  signalmonitorwidget.cpp
)

qt4_wrap_ui(gammaray_signalmonitor_ui_srcs
  signalmonitorwidget.ui
)

gammaray_add_plugin(gammaray_signalmonitor_ui
  gammaray_signalmonitor_ui.desktop
  ${gammaray_signalmonitor_ui_srcs}
)

target_link_libraries(gammaray_signalmonitor_ui
  gammaray_ui
)
