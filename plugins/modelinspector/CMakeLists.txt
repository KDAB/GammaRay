# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_models_srcs
  modelinspector.cpp
  modelinspectorinterface.cpp
  modelmodel.cpp
  modelcellmodel.cpp
  modelcontentproxymodel.cpp
  selectionmodelmodel.cpp
)

gammaray_add_plugin(gammaray_modelinspector
  JSON gammaray_modelinspector.json
  SOURCES ${gammaray_models_srcs}
)

target_link_libraries(gammaray_modelinspector
  gammaray_core
  gammaray_kitemmodels
)
endif()

# ui part
if(GAMMARAY_BUILD_UI)

  set(gammaray_models_ui_srcs
    modelinspectorwidget.cpp
    modelinspectorinterface.cpp
    modelinspectorclient.cpp
    modelcontentdelegate.cpp
  )

  gammaray_add_plugin(gammaray_modelinspector_ui
    JSON gammaray_modelinspector.json
    SOURCES ${gammaray_models_ui_srcs}
  )

  target_link_libraries(gammaray_modelinspector_ui
    gammaray_ui
  )

endif()
