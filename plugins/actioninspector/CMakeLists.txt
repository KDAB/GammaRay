# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_actioninspector_plugin_srcs
  actioninspector.cpp
  actionvalidator.cpp
  actionmodel.cpp
)

gammaray_add_plugin(gammaray_actioninspector_plugin
  DESKTOP gammaray_actioninspector.desktop.in
  JSON gammaray_actioninspector.json
  SOURCES ${gammaray_actioninspector_plugin_srcs}
)

target_link_libraries(gammaray_actioninspector_plugin
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_actioninspector_ui_plugin_srcs
    actioninspectorwidget.cpp
    clientactionmodel.cpp
  )

  gammaray_add_plugin(gammaray_actioninspector_ui_plugin
    DESKTOP gammaray_actioninspector_ui.desktop.in
    JSON gammaray_actioninspector.json
    SOURCES ${gammaray_actioninspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_actioninspector_ui_plugin
    gammaray_ui
  )
endif()
