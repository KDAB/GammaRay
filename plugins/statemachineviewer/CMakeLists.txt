if(GRAPHVIZ_FOUND)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${GRAPHVIZ_INCLUDE_DIR}

  # TODO: Work-around issue in graphviz/types.h header
  # <cgraph.h> is included there, but it should rather be "cgraph.h"
  ${GRAPHVIZ_INCLUDE_DIR}/graphviz
)

if (GRAPHVIZ_CGRAPH_LIBRARY AND GRAPHVIZ_VERSION VERSION_GREATER 2.30.0)
  message(STATUS "Enabling use of experimental 'cgraph' library of GraphViz")
  # you must add this define when using cgraph from graphviz
  # some headers check for this define (see for example graphviz/types.h header)
  add_definitions(-DWITH_CGRAPH)
  set(STATEMACHINEVIEWER_LIBRARIES ${GRAPHVIZ_CGRAPH_LIBRARY})
else()
  set(STATEMACHINEVIEWER_LIBRARIES ${GRAPHVIZ_GRAPH_LIBRARY})
endif()

set(gammaray_statemachineviewer_plugin_srcs
  # this part depends on graphviz
  gvgraph/gvgraph.cpp
  gvgraph/gvgraphitems.cpp
  gvgraph/gvutils.cpp

  statemachineview.cpp
  statemachineviewer.cpp
  statemachinewatcher.cpp

  transitionmodel.cpp
  statemodel.cpp
)

qt4_automoc(${gammaray_statemachineviewer_plugin_srcs})

qt4_wrap_ui(gammaray_statemachineviewer_plugin_srcs statemachineviewer.ui)

set(gammaray_statemachineviewer_plugin_libs
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}

  ${STATEMACHINEVIEWER_LIBRARIES}
  ${GRAPHVIZ_GVC_LIBRARY}
  gammaray_core
)

gammaray_add_plugin(gammaray_statemachineviewer_plugin
  gammaray_statemachineviewer.desktop
  ${gammaray_statemachineviewer_plugin_srcs}
)

target_link_libraries(gammaray_statemachineviewer_plugin
  ${gammaray_statemachineviewer_plugin_libs}
)

add_executable(statemachineviewer_test
  gvgraph/gvgraph.cpp
  gvgraph/gvgraphitems.cpp
  gvgraph/gvutils.cpp

  test_main.cpp
)

target_link_libraries(statemachineviewer_test ${gammaray_statemachineviewer_plugin_libs})

endif()
