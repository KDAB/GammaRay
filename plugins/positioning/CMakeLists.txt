# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_positioning_srcs
  positioning.cpp
)
gammaray_add_plugin(gammaray_positioning JSON gammaray_positioning.json SOURCES ${gammaray_positioning_srcs})
target_link_libraries(gammaray_positioning gammaray_core Qt5::Positioning)
endif()
