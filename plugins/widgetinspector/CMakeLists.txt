# probe plugin
set(gammaray_widgetinspector_plugin_srcs
  widgetinspector.cpp
  widgetinspectorinterface.cpp
  widgetinspectorserver.cpp
  overlaywidget.cpp
  widgettreemodel.cpp
  widgetpaintanalyzerextension.cpp
  widget3dmodel.cpp

  waextension/widgetattributeextension.cpp
)

gammaray_add_plugin(gammaray_widgetinspector_plugin
  DESKTOP gammaray_widgetinspector.desktop.in
  JSON gammaray_widgetinspector.json
  SOURCES ${gammaray_widgetinspector_plugin_srcs}
)

target_link_libraries(gammaray_widgetinspector_plugin
  ${QT_QTGUI_LIBRARIES}
  gammaray_kitemmodels
  gammaray_core
)

# probe export actions
add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

set_target_properties(gammaray_widget_export_actions PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}"
)

if(GAMMARAY_INSTALL_QT_LAYOUT)
  set_target_properties(gammaray_widget_export_actions PROPERTIES OUTPUT_NAME gammaray_widget_export_actions-${GAMMARAY_PROBE_ABI})
endif()

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
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_widget_export_actions>/$<TARGET_PDB_FILE_NAME:gammaray_widget_export_actions>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)

endif()

# UI plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_widgetinspector_ui_plugin_srcs
    widgetinspectorwidget.cpp
    widgetinspectorinterface.cpp
    widgetinspectorclient.cpp
    widgetclientmodel.cpp

    waextension/widgetattributetab.cpp

    assets.qrc
  )

  if (GAMMARAY_WITH_WIDGET3D)
    add_subdirectory(assets)

    set(gammaray_widgetinspector_ui_plugin_srcs
      ${gammaray_widgetinspector_ui_plugin_srcs}
      widget3dview.cpp
      widget3dwindowmodel.cpp
      widget3dsubtreemodel.cpp
      widget3dimagetextureimage.cpp
    )
  endif()

  gammaray_add_plugin(gammaray_widgetinspector_ui_plugin
    DESKTOP gammaray_widgetinspector_ui.desktop.in
    JSON gammaray_widgetinspector.json
    SOURCES ${gammaray_widgetinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_widgetinspector_ui_plugin
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
  )

  if (GAMMARAY_WITH_WIDGET3D)
    target_link_libraries(gammaray_widgetinspector_ui_plugin
        Qt5::Qml
        Qt5::Quick
        Qt5::3DRender
        Qt5::3DLogic
        Qt5::3DInput
        Qt5::3DQuick
    )
  endif()
endif()
