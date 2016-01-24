# probe plugin
set(gammaray_3dinspector_srcs
  3dinspector.cpp
)
gammaray_add_plugin(gammaray_3dinspector gammaray_3dinspector.desktop ${gammaray_3dinspector_srcs})
target_link_libraries(gammaray_3dinspector gammaray_core Qt5::3DRender)
