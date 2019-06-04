# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)

set(gammaray_eventmonitor_plugin_srcs
  eventmonitor.cpp
  eventmodel.cpp
  eventmonitorinterface.cpp
  eventtypemodel.cpp
  eventtypefilter.cpp
)

gammaray_add_plugin(gammaray_eventmonitor_plugin
  JSON gammaray_eventmonitor.json
  SOURCES ${gammaray_eventmonitor_plugin_srcs}
)

target_include_directories(gammaray_eventmonitor_plugin SYSTEM PRIVATE ${Qt5Core_PRIVATE_INCLUDE_DIRS})
target_link_libraries(gammaray_eventmonitor_plugin
  Qt5::Gui
  gammaray_core
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_eventmonitor_ui_plugin_srcs
    eventmonitorwidget.cpp
    eventmonitorinterface.cpp
    eventmonitorclient.cpp
    eventtypeclientproxymodel.cpp
  )

  gammaray_add_plugin(gammaray_eventmonitor_ui_plugin
    JSON gammaray_eventmonitor.json
    SOURCES ${gammaray_eventmonitor_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_eventmonitor_ui_plugin
    gammaray_ui
  )
endif()
