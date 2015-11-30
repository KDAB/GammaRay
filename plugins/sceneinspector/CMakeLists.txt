# probe part
set(gammaray_sceneinspector_plugin_srcs
  scenemodel.cpp
  sceneinspectorinterface.cpp
  sceneinspector.cpp
)

gammaray_add_plugin(gammaray_sceneinspector_plugin
  gammaray_sceneinspector.desktop
  ${gammaray_sceneinspector_plugin_srcs}
)

target_link_libraries(gammaray_sceneinspector_plugin
  ${QT_QTGUI_LIBRARIES}
  gammaray_kitemmodels
  gammaray_core
)

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_sceneinspector_ui_plugin_srcs
    graphicsview.cpp
    graphicssceneview.cpp
    sceneinspectorinterface.cpp
    sceneinspectorclient.cpp
    sceneinspectorwidget.cpp
  )

  qt4_wrap_ui(gammaray_sceneinspector_ui_plugin_srcs
    graphicssceneview.ui
    sceneinspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_sceneinspector_ui_plugin
    gammaray_sceneinspector_ui.desktop
    ${gammaray_sceneinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_sceneinspector_ui_plugin
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )
endif()
