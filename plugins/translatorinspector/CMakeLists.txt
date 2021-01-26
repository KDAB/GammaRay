# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
gammaray_add_plugin(gammaray_translatorinspector
  JSON gammaray_translatorinspector.json
  SOURCES translatorinspector.cpp translatorinspectorinterface.cpp translatorwrapper.cpp translatorsmodel.cpp
)
target_link_libraries(gammaray_translatorinspector gammaray_core Qt::Core Qt::CorePrivate)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  gammaray_add_plugin(gammaray_translatorinspector_ui
    JSON gammaray_translatorinspector.json
    SOURCES
        translatorinspectorwidget.cpp
        translatorinspectorinterface.cpp
        translatorwrapperproxy.cpp
  )
  target_link_libraries(gammaray_translatorinspector_ui gammaray_ui)
endif()
