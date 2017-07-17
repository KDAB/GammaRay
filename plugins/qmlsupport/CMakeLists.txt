# probe plugin
set(gammaray_qmlsupport_srcs
  qmlsupport.cpp
  qmllistpropertyadaptor.cpp
  qmlattachedpropertyadaptor.cpp
  qjsvaluepropertyadaptor.cpp
  qmlcontextpropertyadaptor.cpp
  qmlcontextextension.cpp
  qmlcontextmodel.cpp
  qmltypeextension.cpp
)
include_directories(SYSTEM ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
gammaray_add_plugin(gammaray_qmlsupport JSON gammaray_qmlsupport.json SOURCES ${gammaray_qmlsupport_srcs})
target_link_libraries(gammaray_qmlsupport gammaray_core Qt5::Qml)

# ui plugin
if(GAMMARAY_BUILD_UI)

  set(gammaray_qmlsupport_ui_srcs
    qmlsupportuifactory.cpp
    qmlcontexttab.cpp
    qmltypetab.cpp
  )
  gammaray_add_plugin(gammaray_qmlsupport_ui JSON gammaray_qmlsupport.json SOURCES ${gammaray_qmlsupport_ui_srcs})
  target_link_libraries(gammaray_qmlsupport_ui gammaray_ui)
endif()
