include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_scriptenginedebugger_plugin_srcs
  scriptenginedebugger.cpp
)

qt4_automoc(${gammaray_scriptenginedebugger_plugin_srcs})

qt4_wrap_ui(gammaray_scriptenginedebugger_plugin_srcs scriptenginedebugger.ui)

gammaray_add_plugin(gammaray_scriptenginedebugger_plugin
  gammaray_scriptenginedebugger.desktop
  ${gammaray_scriptenginedebugger_plugin_srcs}
)

target_link_libraries(gammaray_scriptenginedebugger_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSCRIPT_LIBRARIES}
  ${QT_QTSCRIPTTOOLS_LIBRARIES}
  gammaray_probe
)
