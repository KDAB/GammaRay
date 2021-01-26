# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_styleinspector_plugin_srcs
  styleinspector.cpp
  pixelmetricmodel.cpp
  standardiconmodel.cpp
  primitivemodel.cpp
  controlmodel.cpp
  styleoption.cpp
  abstractstyleelementmodel.cpp
  abstractstyleelementstatetable.cpp
  complexcontrolmodel.cpp
  dynamicproxystyle.cpp
  styleinspectorinterface.cpp
  stylehintmodel.cpp

  ${CMAKE_SOURCE_DIR}/ui/palettemodel.cpp
)

gammaray_add_plugin(gammaray_styleinspector_plugin
  JSON gammaray_styleinspector.json
  SOURCES ${gammaray_styleinspector_plugin_srcs}
)

target_link_libraries(gammaray_styleinspector_plugin
  Qt::Gui Qt::Widgets
  gammaray_core
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_styleinspector_ui_plugin_srcs
    styleinspectorwidget.cpp
    styleelementstatetablepage.cpp
    styleinspectorinterface.cpp
    styleinspectorclient.cpp
  )

  gammaray_add_plugin(gammaray_styleinspector_ui_plugin
    JSON gammaray_styleinspector.json
    SOURCES ${gammaray_styleinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_styleinspector_ui_plugin
    gammaray_ui
  )
endif()
