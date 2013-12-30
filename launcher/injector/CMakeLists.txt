if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  set(gammaray_injector_style_srcs injectorstyleplugin.cpp)

  add_library(gammaray_injector_style MODULE ${gammaray_injector_style_srcs})

  target_link_libraries(gammaray_injector_style
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
  )

  set_target_properties(gammaray_injector_style
    PROPERTIES PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}/styles
  )

  install(
    TARGETS gammaray_injector_style
    DESTINATION ${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}/styles
  )
endif()
