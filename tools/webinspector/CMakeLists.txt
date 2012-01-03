include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_webinspector_plugin_srcs
  webinspector.cpp
  ../../singlecolumnobjectproxymodel.cpp
  ../../util.cpp
  ../../3rdparty/kde/kidentityproxymodel.cpp
)
qt4_automoc(${gammaray_webinspector_plugin_srcs})
qt4_wrap_ui(gammaray_webinspector_plugin_srcs webinspector.ui)

gammaray_add_plugin(gammaray_webinspector_plugin gammaray_webinspector.desktop ${gammaray_webinspector_plugin_srcs})
target_link_libraries(gammaray_webinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTWEBKIT_LIBRARIES}
)
