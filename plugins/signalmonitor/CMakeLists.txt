# shared part
set(gammaray_signalmonitor_shared_srcs
  signalmonitorinterface.cpp
  signalmonitorcommon.cpp
)
add_library(gammaray_signalmonitor_shared STATIC ${gammaray_signalmonitor_shared_srcs})
target_link_libraries(gammaray_signalmonitor_shared LINK_PRIVATE gammaray_common)
set_target_properties(gammaray_signalmonitor_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_signalmonitor_shared PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_signalmonitor_srcs
  signalmonitor.cpp
  signalhistorymodel.cpp
  relativeclock.cpp
)

gammaray_add_plugin(gammaray_signalmonitor
  JSON gammaray_signalmonitor.json
  SOURCES ${gammaray_signalmonitor_srcs}
)

target_link_libraries(gammaray_signalmonitor
  gammaray_core
  gammaray_signalmonitor_shared
)
target_link_libraries(gammaray_signalmonitor Qt5::Gui)
endif ()

if(GAMMARAY_BUILD_UI)
  # ui plugin
  set(gammaray_signalmonitor_ui_srcs
    signalmonitorwidget.cpp
    signalhistorydelegate.cpp
    signalhistoryview.cpp
    signalmonitorclient.cpp
  )

  gammaray_add_plugin(gammaray_signalmonitor_ui
    JSON gammaray_signalmonitor.json
    SOURCES ${gammaray_signalmonitor_ui_srcs}
  )

  target_link_libraries(gammaray_signalmonitor_ui
    gammaray_ui
    gammaray_signalmonitor_shared
  )
endif()
