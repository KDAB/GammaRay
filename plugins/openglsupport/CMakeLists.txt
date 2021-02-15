# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
gammaray_add_plugin(gammaray_openglsupport JSON gammaray_openglsupport.json SOURCES openglsupport.cpp)
target_link_libraries(gammaray_openglsupport
    gammaray_core
    Qt::Gui
)
if (TARGET Qt6::OpenGL)
  target_link_libraries(gammaray_openglsupport Qt6::OpenGL)
endif()
endif()

