# probe plugin
set(gammaray_qmlsupport_srcs
  qmlsupport.cpp
  qmllistpropertyadaptor.cpp
  qmlattachedpropertyadaptor.cpp
  qjsvaluepropertyadaptor.cpp
)
include_directories(SYSTEM ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
gammaray_add_plugin(gammaray_qmlsupport JSON gammaray_qmlsupport.json SOURCES ${gammaray_qmlsupport_srcs})
target_link_libraries(gammaray_qmlsupport gammaray_core Qt5::Qml)
