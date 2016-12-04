# shared part
set(gammaray_statemachineviewer_shared_srcs
  statemachineviewerinterface.cpp
)


# probe part
set(gammaray_statemachineviewer_plugin_srcs
  statemachineviewerserver.cpp
  transitionmodel.cpp
  statemodel.cpp
  statemachinewatcher.cpp
  statemachinedebuginterface.cpp
  qsmstatemachinedebuginterface.cpp
)

gammaray_add_plugin(gammaray_statemachineviewer_plugin
  DESKTOP gammaray_statemachineviewer.desktop.in
  JSON gammaray_statemachineviewer.json
  SOURCES ${gammaray_statemachineviewer_plugin_srcs} ${gammaray_statemachineviewer_shared_srcs}
)

target_link_libraries(gammaray_statemachineviewer_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

# UI part - dependencies
set_package_properties(KDSME PROPERTIES URL "https://github.com/KDAB/KDStateMachineEditor" DESCRIPTION "KDAB State Machine Editor framework" TYPE RECOMMENDED PURPOSE "Graphical state machine debugging.")
if (Qt5_FOUND)
  find_package(KDSME 1.1 CONFIG QUIET)
else()
  add_feature_info("State machine debugging" FALSE "Graphical state machine debugging is not available when building with Qt4.")
endif()

# UI part
if(GAMMARAY_BUILD_UI AND KDSME_FOUND)

set(gammaray_statemachineviewer_ui_plugin_srcs
  statemachineviewerwidget.cpp
  statemachineviewerclient.cpp
  statemodeldelegate.cpp
)

qt4_wrap_ui(gammaray_statemachineviewer_ui_plugin_srcs statemachineviewerwidget.ui)

gammaray_add_plugin(gammaray_statemachineviewer_ui_plugin
  JSON gammaray_statemachineviewer.json
  SOURCES ${gammaray_statemachineviewer_ui_plugin_srcs} ${gammaray_statemachineviewer_shared_srcs}
)

target_link_libraries(gammaray_statemachineviewer_ui_plugin
  KDSME::Core
  KDSME::View
  gammaray_common
  gammaray_ui
)

endif()
