if(HAVE_QT_WEBKIT)
  # probe part
  if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
  set(gammaray_webinspector_plugin_srcs
    webinspector.cpp
    webviewmodel.cpp
  )

  gammaray_add_plugin(gammaray_webinspector_plugin
    DESKTOP gammaray_webinspector.desktop.in
    JSON gammaray_webinspector.json
    SOURCES ${gammaray_webinspector_plugin_srcs}
  )

  target_link_libraries(gammaray_webinspector_plugin
    gammaray_core
  )
  endif()

  if(HAVE_QT_WEBKIT1)
    if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
    target_link_libraries(gammaray_webinspector_plugin
      ${QT_QTWEBKIT_LIBRARIES}
      ${Qt5WebKitWidgets_LIBRARIES}
    )
    endif()

    if(GAMMARAY_BUILD_UI)
      # ui part
      set(gammaray_webinspector_ui_plugin_srcs
        webinspectorwidget.cpp
      )

      gammaray_add_plugin(gammaray_webinspector_ui_plugin
        DESKTOP gammaray_webinspector_ui.desktop.in
        JSON gammaray_webinspector.json
        SOURCES ${gammaray_webinspector_ui_plugin_srcs}
      )

      target_link_libraries(gammaray_webinspector_ui_plugin
        ${QT_QTWEBKIT_LIBRARIES}
        ${Qt5WebKitWidgets_LIBRARIES}
        gammaray_ui
      )
    endif()
  endif()
endif()
