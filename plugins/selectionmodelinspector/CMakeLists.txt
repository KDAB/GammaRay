# probe part
set(gammaray_selectionmodelinspector_plugin_srcs
  selectionmodelinspector.cpp
)

gammaray_add_plugin(gammaray_selectionmodelinspector_plugin
  gammaray_selectionmodelinspector.desktop
  ${gammaray_selectionmodelinspector_plugin_srcs}
)

target_link_libraries(gammaray_selectionmodelinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

# ui part
if((Qt5Widgets_FOUND OR QT_QTGUI_FOUND) AND NOT GAMMARAY_PROBE_ONLY_BUILD)

  set(gammaray_selectionmodelinspector_plugin_ui_srcs
    selectionmodelinspectorwidget.cpp
  )

  qt4_wrap_ui(gammaray_selectionmodelinspector_plugin_ui_srcs
    selectionmodelinspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_selectionmodelinspector_ui_plugin
    gammaray_selectionmodelinspector_ui.desktop
    ${gammaray_selectionmodelinspector_plugin_ui_srcs}
  )

  target_link_libraries(gammaray_selectionmodelinspector_ui_plugin
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_common
  )

endif()
