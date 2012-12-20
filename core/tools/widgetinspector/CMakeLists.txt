include_directories(${CMAKE_BINARY_DIR})

add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

target_link_libraries(gammaray_widget_export_actions
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSVG_LIBRARIES}
  ${QT_QTDESIGNER_LIBRARIES}
)

install(TARGETS gammaray_widget_export_actions ${INSTALL_TARGETS_DEFAULT_ARGS})
