# probe part
set(gammaray_selectionmodelinspector_plugin_srcs
  selectionmodelinspector.cpp
)

gammaray_add_plugin(gammaray_selectionmodelinspector_plugin
  DESKTOP gammaray_selectionmodelinspector.desktop.in
  JSON gammaray_selectionmodelinspector.json
  SOURCES ${gammaray_selectionmodelinspector_plugin_srcs}
)

target_link_libraries(gammaray_selectionmodelinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_selectionmodelinspector_plugin_ui_srcs
    selectionmodelinspectorwidget.cpp
  )

  qt4_wrap_ui(gammaray_selectionmodelinspector_plugin_ui_srcs
    selectionmodelinspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_selectionmodelinspector_ui_plugin
    DESKTOP gammaray_selectionmodelinspector_ui.desktop.in
    JSON gammaray_selectionmodelinspector.json
    SOURCES ${gammaray_selectionmodelinspector_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_selectionmodelinspector_ui_plugin
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )

endif()
