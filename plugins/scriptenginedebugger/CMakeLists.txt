# probe plugin
set(gammaray_scriptenginedebugger_plugin_srcs
  scriptenginedebugger.cpp
)

gammaray_add_plugin(gammaray_scriptenginedebugger_plugin
  gammaray_scriptenginedebugger.desktop
  ${gammaray_scriptenginedebugger_plugin_srcs}
)

target_link_libraries(gammaray_scriptenginedebugger_plugin
  ${QT_QTSCRIPT_LIBRARIES}
  ${QT_QTSCRIPTTOOLS_LIBRARIES}
  gammaray_core
)

# UI plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_scriptenginedebugger_ui_plugin_srcs
    scriptenginedebuggerwidget.cpp
  )

  qt4_wrap_ui(gammaray_scriptenginedebugger_ui_plugin_srcs scriptenginedebuggerwidget.ui)

  gammaray_add_plugin(gammaray_scriptenginedebugger_ui_plugin
    gammaray_scriptenginedebugger_ui.desktop
    ${gammaray_scriptenginedebugger_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_scriptenginedebugger_ui_plugin
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTSCRIPT_LIBRARIES}
    ${QT_QTSCRIPTTOOLS_LIBRARIES}
    gammaray_core
  )
endif()
