# shared part
set(gammaray_signalmonitor_shared_srcs
  signalmonitorinterface.cpp
  signalmonitorcommon.cpp
)
add_library(gammaray_signalmonitor_shared STATIC ${gammaray_signalmonitor_shared_srcs})
target_link_libraries(gammaray_signalmonitor_shared LINK_PRIVATE gammaray_common)
set_target_properties(gammaray_signalmonitor_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

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

target_link_libraries(gammaray_signalmonitor
  gammaray_core
  gammaray_signalmonitor_shared
)
if (Qt5_FOUND)
  target_link_libraries(gammaray_signalmonitor Qt5::Gui)
endif ()

if(GAMMARAY_BUILD_UI)
  # ui plugin
  set(gammaray_signalmonitor_ui_srcs
    signalhistorydelegate.cpp
    signalhistoryview.cpp
    signalmonitorwidget.cpp
    signalmonitorclient.cpp
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
    gammaray_signalmonitor_shared
  )
endif()
