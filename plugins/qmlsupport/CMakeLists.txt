# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
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

gammaray_add_plugin(gammaray_qmlsupport JSON gammaray_qmlsupport.json SOURCES ${gammaray_qmlsupport_srcs})
target_include_directories(gammaray_qmlsupport SYSTEM PRIVATE ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
target_link_libraries(gammaray_qmlsupport gammaray_core Qt5::Qml)
endif()


if(NOT Qt5Qml_VERSION VERSION_LESS 5.10)
    target_sources(gammaray_qmlsupport PUBLIC qmlbindingprovider.cpp)
endif()

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
