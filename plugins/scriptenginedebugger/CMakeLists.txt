set(gammaray_scriptenginedebugger_plugin_srcs
  scriptenginedebugger.cpp
  scriptenginedebuggerwidget.cpp
)

qt4_wrap_ui(gammaray_scriptenginedebugger_plugin_srcs scriptenginedebuggerwidget.ui)

gammaray_add_plugin(gammaray_scriptenginedebugger_plugin
  gammaray_scriptenginedebugger.desktop
  ${gammaray_scriptenginedebugger_plugin_srcs}
)

target_link_libraries(gammaray_scriptenginedebugger_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSCRIPT_LIBRARIES}
  ${QT_QTSCRIPTTOOLS_LIBRARIES}
  gammaray_core
)
