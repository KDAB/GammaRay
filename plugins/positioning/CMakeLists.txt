# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_positioning_srcs
  positioning.cpp
  positioninginterface.cpp
)
gammaray_add_plugin(gammaray_positioning JSON gammaray_positioning.json SOURCES ${gammaray_positioning_srcs})
target_link_libraries(gammaray_positioning gammaray_core Qt5::Positioning)
endif()

# proxy geo position info source factory
set(gammaray_geopositioninfosource_srcs
  geopositioninfosourcefactory.cpp
  geopositioninfosource.cpp
  positioninginterface.cpp
)

add_library(gammaray_geopositioninfosource MODULE ${gammaray_geopositioninfosource_srcs})
target_link_libraries(gammaray_geopositioninfosource
  gammaray_common
  Qt5::Positioning
)
install(TARGETS gammaray_geopositioninfosource DESTINATION ${PROBE_PLUGIN_INSTALL_DIR}/position)

# ui plugin
if(GAMMARAY_BUILD_UI AND Qt5Location_FOUND AND Qt5QuickWidgets_FOUND)
  set(gammaray_positioning_ui_srcs
    positioningwidget.cpp
    positioninginterface.cpp
    positioningclient.cpp
    mapcontroller.cpp
  )

  qt4_wrap_ui(gammaray_positioning_ui_srcs
    positioningwidget.ui
  )
  qt4_add_resources(gammaray_positioning_ui_srcs positioningwidget.qrc)

  gammaray_add_plugin(gammaray_positioning_ui
    gammaray_positioning_ui.desktop
    ${gammaray_positioning_ui_srcs}
  )

  target_link_libraries(gammaray_positioning_ui
    gammaray_ui
    Qt5::Widgets
    Qt5::Positioning
    Qt5::Location
    Qt5::QuickWidgets
  )
endif()
