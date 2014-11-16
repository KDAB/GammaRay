# probe part
set(gammaray_actioninspector_plugin_srcs
  actionvalidator.cpp
  actioninspector.cpp
  actionmodel.cpp
)

gammaray_add_plugin(gammaray_actioninspector_plugin
  gammaray_actioninspector.desktop
  ${gammaray_actioninspector_plugin_srcs}
)

target_link_libraries(gammaray_actioninspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
)

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_actioninspector_ui_plugin_srcs
    actioninspectorwidget.cpp
  )

  gammaray_add_plugin(gammaray_actioninspector_ui_plugin
    gammaray_actioninspector_ui.desktop
    ${gammaray_actioninspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_actioninspector_ui_plugin
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )
endif()
