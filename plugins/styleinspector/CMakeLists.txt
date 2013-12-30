set(gammaray_styleinspector_plugin_srcs
  styleinspector.cpp
  styleinspectorwidget.cpp
  pixelmetricmodel.cpp
  standardiconmodel.cpp
  primitivemodel.cpp
  controlmodel.cpp
  styleoption.cpp
  abstractstyleelementmodel.cpp
  abstractstyleelementstatetable.cpp
  styleelementstatetablepage.cpp
  complexcontrolmodel.cpp
  dynamicproxystyle.cpp
  styleinspectorinterface.cpp
  styleinspectorclient.cpp
)

qt4_wrap_ui(gammaray_styleinspector_plugin_srcs
  styleinspectorwidget.ui
  styleelementstatetablepage.ui
)

gammaray_add_plugin(gammaray_styleinspector_plugin
  gammaray_styleinspector.desktop
  ${gammaray_styleinspector_plugin_srcs}
)

target_link_libraries(gammaray_styleinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
  gammaray_ui
)

if(QNXNTO)
  target_link_libraries(gammaray_styleinspector_plugin cpp)
endif()
