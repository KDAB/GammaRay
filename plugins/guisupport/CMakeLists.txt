# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_guisupport_srcs
  guisupport.cpp

  ui.qrc
)
gammaray_add_plugin(gammaray_guisupport JSON gammaray_guisupport.json SOURCES ${gammaray_guisupport_srcs})
target_link_libraries(gammaray_guisupport gammaray_core)
target_include_directories(gammaray_guisupport SYSTEM PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
target_link_libraries(gammaray_guisupport Qt5::Gui)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_guisupport_ui_srcs
    guisupportuifactory.cpp
    paintanalyzertab.cpp
  )
  gammaray_add_plugin(gammaray_guisupport_ui JSON gammaray_guisupport.json SOURCES ${gammaray_guisupport_ui_srcs})
  target_link_libraries(gammaray_guisupport_ui gammaray_ui)
endif()
