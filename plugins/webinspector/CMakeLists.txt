# probe part
set(gammaray_webinspector_plugin_srcs
  webinspector.cpp
  webviewmodel.cpp
)

gammaray_add_plugin(gammaray_webinspector_plugin
  gammaray_webinspector.desktop
  ${gammaray_webinspector_plugin_srcs}
)

target_link_libraries(gammaray_webinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

if(HAVE_QT_WEBKIT1)
  target_link_libraries(gammaray_webinspector_plugin
    ${QT_QTWEBKIT_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
  )

  if(GAMMARAY_BUILD_UI)
    # ui part
    set(gammaray_webinspector_ui_plugin_srcs
      webinspectorwidget.cpp
    )

    qt4_wrap_ui(gammaray_webinspector_ui_plugin_srcs webinspectorwidget.ui)

    gammaray_add_plugin(gammaray_webinspector_ui_plugin
      gammaray_webinspector_ui.desktop
      ${gammaray_webinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_ui_plugin
      ${QT_QTCORE_LIBRARIES}
      ${QT_QTGUI_LIBRARIES}
      ${QT_QTWEBKIT_LIBRARIES}
      ${Qt5WebKitWidgets_LIBRARIES}
      gammaray_common
    )
  endif()

endif()
