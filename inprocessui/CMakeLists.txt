add_library(gammaray_inprocessui MODULE main.cpp)
target_link_libraries(gammaray_inprocessui gammaray_core gammaray_ui_internal)
set_target_properties(gammaray_inprocessui PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PROBE_PLUGIN_INSTALL_DIR}"
)
install(TARGETS gammaray_inprocessui DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
