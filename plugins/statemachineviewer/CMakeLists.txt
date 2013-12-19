include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}
)

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
)

gammaray_add_plugin(gammaray_statemachineviewer_plugin
  gammaray_statemachineviewer.desktop
  ${gammaray_statemachineviewer_shared_srcs}
  ${gammaray_statemachineviewer_plugin_srcs}
)

target_link_libraries(gammaray_statemachineviewer_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

############ BEGIN GRAPHVIZ GUI
if(GRAPHVIZ_FOUND AND NOT GAMMARAY_PROBE_ONLY_BUILD)

set(gammaray_statemachineviewer_ui_plugin_libs
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}

  gammaray_common
  gammaray_ui
)

set(gammaray_statemachineviewer_ui_plugin_srcs
  # gui & client
  statemachineviewerclient.cpp
  statemachineview.cpp
  statemachineviewerwidget.cpp
  # this part depends on graphviz
  gvgraph/gvgraph.cpp
  gvgraph/gvgraphitems.cpp
  gvgraph/gvutils.cpp
)

include_directories(
  ${GRAPHVIZ_INCLUDE_DIR}

  # TODO: Work-around issue in graphviz/types.h header
  # <cgraph.h> is included there, but it should rather be "cgraph.h"
  ${GRAPHVIZ_INCLUDE_DIR}/graphviz
)

list(APPEND gammaray_statemachineviewer_ui_plugin_libs
  ${GRAPHVIZ_GVC_LIBRARY}
)

if(GRAPHVIZ_CGRAPH_LIBRARY AND GRAPHVIZ_VERSION VERSION_GREATER 2.30.0)
  message(STATUS "Enabling use of experimental 'cgraph' library of GraphViz")
  # you must add this define when using cgraph from graphviz
  # some headers check for this define (see for example graphviz/types.h header)
  add_definitions(-DWITH_CGRAPH)
  list(APPEND gammaray_statemachineviewer_ui_plugin_libs
    ${GRAPHVIZ_CGRAPH_LIBRARY}
  )
else()
  list(APPEND gammaray_statemachineviewer_ui_plugin_libs
    ${GRAPHVIZ_GRAPH_LIBRARY}
  )
endif()

qt4_wrap_ui(gammaray_statemachineviewer_ui_plugin_srcs statemachineviewer.ui)

add_executable(statemachineviewer_test
  gvgraph/gvgraph.cpp
  gvgraph/gvgraphitems.cpp
  gvgraph/gvutils.cpp

  test_main.cpp
)

target_link_libraries(statemachineviewer_test ${gammaray_statemachineviewer_ui_plugin_libs})

gammaray_add_plugin(gammaray_statemachineviewer_ui_plugin
  gammaray_statemachineviewer_ui.desktop
  ${gammaray_statemachineviewer_shared_srcs}
  ${gammaray_statemachineviewer_ui_plugin_srcs}
)

target_link_libraries(gammaray_statemachineviewer_ui_plugin
  ${gammaray_statemachineviewer_ui_plugin_libs}
)

endif()

################### END GRAPHVIZ GUI
