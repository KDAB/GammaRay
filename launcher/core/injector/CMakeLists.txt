if(Qt5Widgets_FOUND)
  set(gammaray_injector_style_srcs injectorstyleplugin.cpp)

  add_library(gammaray_injector_style MODULE ${gammaray_injector_style_srcs})

  target_include_directories(gammaray_injector_style SYSTEM PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
  target_link_libraries(gammaray_injector_style
    Qt5::Core
    Qt5::Gui Qt5::Widgets
  )

  set_target_properties(gammaray_injector_style
    PROPERTIES PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}/styles
  )
  gammaray_set_rpath(gammaray_injector_style ${PROBE_PLUGIN_INSTALL_DIR}/styles)
  target_compile_features(gammaray_injector_style PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

  if(GAMMARAY_INSTALL_QT_LAYOUT)
    set_target_properties(gammaray_injector_style PROPERTIES OUTPUT_NAME gammaray_injector_style-${GAMMARAY_PROBE_ABI})
  endif()

  install(
    TARGETS gammaray_injector_style
    DESTINATION ${PROBE_PLUGIN_INSTALL_DIR}/styles
  )
  if(MSVC)
    install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_injector_style>/$<TARGET_PDB_FILE_NAME:gammaray_injector_style>" DESTINATION "${PROBE_PLUGIN_INSTALL_DIR}/styles" CONFIGURATIONS Debug RelWithDebInfo)
  endif()
endif()
