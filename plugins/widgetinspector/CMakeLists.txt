if(NOT Qt5Gui_VERSION VERSION_LESS 5.5.0) # QPaintBuffer was removed in 5.5
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/)
endif()

# probe plugin
set(gammaray_widgetinspector_plugin_srcs
  widgetinspector.cpp
  widgetinspectorinterface.cpp
  widgetinspectorserver.cpp
  overlaywidget.cpp
  widgettreemodel.cpp
  paintbufferreplaywidget.cpp
  modelutils.cpp
)

if(HAVE_PRIVATE_QT_HEADERS)
  set(gammaray_widgetinspector_plugin_srcs
    ${gammaray_widgetinspector_plugin_srcs}
    paintbuffermodel.cpp
  )
endif()

if(NOT Qt5Gui_VERSION VERSION_LESS 5.5.0)
  list(APPEND gammaray_widgetinspector_plugin_srcs ${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/private/qpaintbuffer.cpp)
endif()

if(NOT Qt5Gui_VERSION VERSION_LESS 5.5.0)
  list(APPEND gammaray_widgetinspector_plugin_srcs ${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/private/qpaintbuffer.cpp)
endif()

gammaray_add_plugin(gammaray_widgetinspector_plugin
  gammaray_widgetinspector.desktop
  ${gammaray_widgetinspector_plugin_srcs}
)

target_link_libraries(gammaray_widgetinspector_plugin
  ${QT_QTGUI_LIBRARIES}
  gammaray_core
)

# probe export actions
add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

set_target_properties(gammaray_widget_export_actions PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PROBE_PLUGIN_INSTALL_DIR}"
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

install(TARGETS gammaray_widget_export_actions DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})

# UI plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_widgetinspector_ui_plugin_srcs
    widgetinspectorinterface.cpp
    widgetinspectorclient.cpp
    widgetinspectorwidget.cpp
    widgetpreviewwidget.cpp
    paintbufferviewer.cpp
    paintbufferreplaywidget.cpp
  )

  qt4_wrap_ui(gammaray_widgetinspector_ui_plugin_srcs
    widgetinspectorwidget.ui
    paintbufferviewer.ui
  )

  gammaray_add_plugin(gammaray_widgetinspector_ui_plugin
    gammaray_widgetinspector_ui.desktop
    ${gammaray_widgetinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_widgetinspector_ui_plugin
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )
endif()
