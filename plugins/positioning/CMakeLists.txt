# probe plugin
set(gammaray_positioning_srcs
  positioning.cpp
)
gammaray_add_plugin(gammaray_positioning gammaray_positioning.desktop ${gammaray_positioning_srcs})
target_link_libraries(gammaray_positioning gammaray_core Qt5::Positioning)
