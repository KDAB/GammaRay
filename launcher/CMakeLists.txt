if(NOT GAMMARAY_PROBE_ONLY_BUILD)
add_subdirectory(core)
add_subdirectory(cli)
if(HAVE_QT_CONCURRENT AND GAMMARAY_BUILD_UI)
  add_subdirectory(ui)
  add_subdirectory(app)
endif()
endif()

if(GAMMARAY_BUILD_CLI_INJECTOR)
  add_subdirectory(win-injector)
endif()

