# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
find_package(Wayland 1.12 COMPONENTS Server)
set_package_properties(Wayland PROPERTIES PURPOSE "Needed for the QtWayland compositor inspector plug-in." TYPE OPTIONAL)

if(TARGET Qt::WaylandCompositor AND Wayland_FOUND)
  set(gammaray_wlcompositorinspector_srcs
    wlcompositorinspector.cpp
    wlcompositorinterface.cpp
    clientsmodel.cpp
    resourceinfo.cpp
  )
  gammaray_add_plugin(gammaray_wlcompositorinspector
    JSON gammaray_wlcompositorinspector.json SOURCES
    ${gammaray_wlcompositorinspector_srcs}
  )
  target_link_libraries(gammaray_wlcompositorinspector
    gammaray_core gammaray_kitemmodels Qt::WaylandCompositor Wayland::Server)

endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI AND (NOT MSVC OR NOT MSVC_VERSION LESS 1800)) # at least MSVC 2013

  set(gammaray_wlcompositorinspector_ui_srcs
    inspectorwidget.cpp
    wlcompositorinterface.cpp
    wlcompositorclient.cpp
    logview.cpp
  )

  gammaray_add_plugin(gammaray_wlcompositorinspector_ui
    JSON gammaray_wlcompositorinspector.json
    SOURCES ${gammaray_wlcompositorinspector_ui_srcs}
  )


  target_link_libraries(gammaray_wlcompositorinspector_ui
    Qt::Widgets
    gammaray_common
    gammaray_ui
  )

endif()
