# probe plugin
set(gammaray_qmlsupport_srcs
  qmlsupport.cpp
  qmllistpropertyadaptor.cpp
  qmlattachedpropertyadaptor.cpp
)
include_directories(${Qt5Qml_PRIVATE_INCLUDE_DIRS})
gammaray_add_plugin(gammaray_qmlsupport gammaray_qmlsupport.desktop ${gammaray_qmlsupport_srcs})
target_link_libraries(gammaray_qmlsupport gammaray_core Qt5::Qml)
