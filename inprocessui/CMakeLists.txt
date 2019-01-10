add_library(gammaray_inprocessui MODULE main.cpp)
target_link_libraries(gammaray_inprocessui gammaray_core gammaray_ui_internal)
set_target_properties(gammaray_inprocessui PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}"
)
gammaray_set_rpath(gammaray_inprocessui ${PROBE_PLUGIN_INSTALL_DIR})
target_compile_features(gammaray_inprocessui PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

if(GAMMARAY_INSTALL_QT_LAYOUT)
  set_target_properties(gammaray_inprocessui PROPERTIES OUTPUT_NAME gammaray_inprocessui-${GAMMARAY_PROBE_ABI})
endif()

install(TARGETS gammaray_inprocessui DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_inprocessui>/$<TARGET_PDB_FILE_NAME:gammaray_inprocessui>" DESTINATION ${PROBE_PLUGIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
