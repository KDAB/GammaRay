include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_webinspector_plugin_srcs
  webinspector.cpp
)
qt4_automoc(${gammaray_webinspector_plugin_srcs})
qt4_wrap_ui(gammaray_webinspector_plugin_srcs webinspector.ui)

add_library(gammaray_webinspector_plugin MODULE ${gammaray_webinspector_plugin_srcs})
target_link_libraries(gammaray_webinspector_plugin
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTWEBKIT_LIBRARY}
)
set_target_properties(gammaray_webinspector_plugin
  PROPERTIES PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/plugins
)
install(
  TARGETS gammaray_webinspector_plugin
  DESTINATION ${PLUGIN_INSTALL_DIR}/gammaray/
)
