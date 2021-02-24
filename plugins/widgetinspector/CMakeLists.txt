# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
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
  JSON gammaray_widgetinspector.json
  SOURCES ${gammaray_widgetinspector_plugin_srcs}
)

target_link_libraries(gammaray_widgetinspector_plugin
  Qt::Gui Qt::Widgets
  gammaray_kitemmodels
  gammaray_core
)
endif()

# probe export actions
add_library(gammaray_widget_export_actions SHARED
  widgetinspector_export_actions.cpp
  uiextractor.cpp
)

set_target_properties(gammaray_widget_export_actions PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_PLUGIN_INSTALL_DIR}"
  PREFIX "lib"
)
gammaray_set_rpath(gammaray_widget_export_actions ${PROBE_PLUGIN_INSTALL_DIR})
target_compile_features(gammaray_widget_export_actions PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

if(GAMMARAY_INSTALL_QT_LAYOUT)
  set_target_properties(gammaray_widget_export_actions PROPERTIES OUTPUT_NAME gammaray_widget_export_actions-${GAMMARAY_PROBE_ABI})
endif()

target_link_libraries(gammaray_widget_export_actions Qt::Widgets)
if (TARGET Qt::Designer)
    target_link_libraries(gammaray_widget_export_actions Qt::Designer)
endif()
if (TARGET Qt::Svg)
    target_link_libraries(gammaray_widget_export_actions Qt::Svg)
endif()

install(TARGETS gammaray_widget_export_actions DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_widget_export_actions>/$<TARGET_PDB_FILE_NAME:gammaray_widget_export_actions>" DESTINATION ${PROBE_PLUGIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()

# UI plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_widgetinspector_ui_plugin_srcs
    widgetinspectorwidget.cpp
    widgetinspectorinterface.cpp
    widgetinspectorclient.cpp
    widgetclientmodel.cpp
    widgetremoteview.cpp

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
    JSON gammaray_widgetinspector.json
    SOURCES ${gammaray_widgetinspector_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_widgetinspector_ui_plugin
    gammaray_ui
  )

  if (GAMMARAY_WITH_WIDGET3D)
    target_link_libraries(gammaray_widgetinspector_ui_plugin
        Qt::Qml
        Qt::Quick
        Qt::3DRender
        Qt::3DLogic
        Qt::3DInput
        Qt::3DQuick
    )
  endif()
endif()
