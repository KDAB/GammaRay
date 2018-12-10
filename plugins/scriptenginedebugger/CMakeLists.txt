# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_scriptenginedebugger_plugin_srcs
  scriptenginedebugger.cpp
)

gammaray_add_plugin(gammaray_scriptenginedebugger_plugin
  DESKTOP gammaray_scriptenginedebugger.desktop.in
  JSON gammaray_scriptenginedebugger.json
  SOURCES ${gammaray_scriptenginedebugger_plugin_srcs}
)

target_link_libraries(gammaray_scriptenginedebugger_plugin
  ${Qt5Script_LIBRARIES}
  ${Qt5ScriptTools_LIBRARIES}
  gammaray_core
)
endif()

# UI plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_scriptenginedebugger_ui_plugin_srcs
    scriptenginedebuggerwidget.cpp
  )

  gammaray_add_plugin(gammaray_scriptenginedebugger_ui_plugin
    DESKTOP gammaray_scriptenginedebugger_ui.desktop.in
    JSON gammaray_scriptenginedebugger.json
    SOURCES ${gammaray_scriptenginedebugger_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_scriptenginedebugger_ui_plugin
    ${Qt5Script_LIBRARIES}
    ${Qt5ScriptTools_LIBRARIES}
    gammaray_ui
  )
endif()
