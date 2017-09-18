# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_sceneinspector_plugin_srcs
  sceneinspector.cpp
  scenemodel.cpp
  sceneinspectorinterface.cpp
  paintanalyzerextension.cpp
)

gammaray_add_plugin(gammaray_sceneinspector_plugin
  DESKTOP gammaray_sceneinspector.desktop.in
  JSON gammaray_sceneinspector.json
  SOURCES ${gammaray_sceneinspector_plugin_srcs}
)

target_link_libraries(gammaray_sceneinspector_plugin
  ${QT_QTGUI_LIBRARIES}
  gammaray_kitemmodels
  gammaray_core
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_sceneinspector_ui_plugin_srcs
    sceneinspectorwidget.cpp
    graphicsview.cpp
    graphicssceneview.cpp
    sceneinspectorinterface.cpp
    sceneinspectorclient.cpp
  )

  gammaray_add_plugin(gammaray_sceneinspector_ui_plugin
    DESKTOP gammaray_sceneinspector_ui.desktop.in
    JSON gammaray_sceneinspector.json
    SOURCES ${gammaray_sceneinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_sceneinspector_ui_plugin
    gammaray_ui
  )
endif()
