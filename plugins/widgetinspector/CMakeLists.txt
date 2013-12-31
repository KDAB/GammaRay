set(gammaray_widgetinspector_plugin_srcs
  widgetinspector.cpp
  widgetinspectorinterface.cpp
  widgetinspectorserver.cpp
  widgetinspectorclient.cpp
  widgetinspectorwidget.cpp
  overlaywidget.cpp
  widgettreemodel.cpp
  widgetpreviewwidget.cpp
  paintbufferviewer.cpp
  paintbufferreplaywidget.cpp
)

if(HAVE_PRIVATE_QT_HEADERS)
  set(gammaray_widgetinspector_plugin_srcs
    ${gammaray_widgetinspector_plugin_srcs}
    paintbuffermodel.cpp
  )
endif()

qt4_wrap_ui(gammaray_widgetinspector_plugin_srcs
  widgetinspectorwidget.ui
  paintbufferviewer.ui
)

gammaray_add_plugin(gammaray_widgetinspector_plugin
  gammaray_widgetinspector.desktop
  ${gammaray_widgetinspector_plugin_srcs}
)

target_link_libraries(gammaray_widgetinspector_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
  gammaray_ui
)

add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

set_target_properties(gammaray_widget_export_actions PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}"
)

set(EXTRA_LIBS)
if(HAVE_QT_DESIGNER)
  list(APPEND EXTRA_LIBS ${QT_QTDESIGNER_LIBRARIES})
endif()

target_link_libraries(gammaray_widget_export_actions
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSVG_LIBRARIES}
  ${Qt5PrintSupport_LIBRARIES}

  ${EXTRA_LIBS}
)

install(TARGETS gammaray_widget_export_actions DESTINATION ${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI})
