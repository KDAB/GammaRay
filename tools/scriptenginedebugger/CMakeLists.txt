include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_scriptenginedebugger_plugin_srcs
  scriptenginedebugger.cpp
)
qt4_automoc(${gammaray_scriptenginedebugger_plugin_srcs})
qt4_wrap_ui(gammaray_scriptenginedebugger_plugin_srcs scriptenginedebugger.ui)

add_library(gammaray_scriptenginedebugger_plugin MODULE ${gammaray_scriptenginedebugger_plugin_srcs})
target_link_libraries(gammaray_scriptenginedebugger_plugin
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTSCRIPTTOOLS_LIBRARY}
)
set_target_properties(gammaray_scriptenginedebugger_plugin
  PROPERTIES PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/plugins
)
install(
  TARGETS gammaray_scriptenginedebugger_plugin
  DESTINATION ${PLUGIN_INSTALL_DIR}/gammaray/
)
