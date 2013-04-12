include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/ui
)

set(gammaray_widgetinspector_plugin_srcs
  widgetinspector.cpp
  widgetinspectorwidget.cpp
  overlaywidget.cpp
  widgettreemodel.cpp
  widgetpreviewwidget.cpp
)

if(HAVE_PRIVATE_QT_HEADERS)
  set(gammaray_widgetinspector_plugin_srcs
    ${gammaray_widgetinspector_plugin_srcs}
    paintbuffermodel.cpp
    paintbufferviewer.cpp
    paintbufferreplaywidget.cpp
  )
endif()

qt4_wrap_ui(gammaray_widgetinspector_plugin_srcs
  widgetinspectorwidget.ui
  paintbufferviewer.ui
)

qt4_automoc(${gammaray_widgetinspector_plugin_srcs})

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

install(TARGETS gammaray_widgetinspector_plugin ${INSTALL_TARGETS_DEFAULT_ARGS})


add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

set(EXTRA_LIBS)
if (HAVE_QT_DESIGNER)
  list(APPEND EXTRA_LIBS ${QT_QTDESIGNER_LIBRARIES})
endif()

target_link_libraries(gammaray_widget_export_actions
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSVG_LIBRARIES}

  ${EXTRA_LIBS}
)

install(TARGETS gammaray_widget_export_actions ${INSTALL_TARGETS_DEFAULT_ARGS})
