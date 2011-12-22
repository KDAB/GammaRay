add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

target_link_libraries(gammaray_widget_export_actions
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSVG_LIBRARY}
  ${QT_QTDESIGNER_LIBRARY}
)

install(TARGETS gammaray_widget_export_actions ${INSTALL_TARGETS_DEFAULT_ARGS})
