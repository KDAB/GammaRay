# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_quickwidgetsupport_srcs
  quickwidgetsupport.cpp
)
gammaray_add_plugin(gammaray_quickwidgetsupport JSON gammaray_quickwidgetsupport.json SOURCES ${gammaray_quickwidgetsupport_srcs})
target_link_libraries(gammaray_quickwidgetsupport gammaray_core Qt::QuickWidgets)
endif()
