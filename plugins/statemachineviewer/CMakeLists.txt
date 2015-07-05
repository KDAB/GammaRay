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


# UI part - dependencies
set(GRAPHVIZ_MIN_VERSION "2.20")
find_package(Graphviz)
set_package_properties(Graphviz PROPERTIES
  TYPE RECOMMENDED
  DESCRIPTION "Graph visualization software"
  URL "http://www.graphviz.org/"
  PURPOSE "Needed for the state machine visualizer plugin."
)
set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
if(GRAPHVIZ_FOUND)
  add_definitions(-DGRAPHVIZ_MAJOR_VERSION=${GRAPHVIZ_MAJOR_VERSION} -DGRAPHVIZ_MINOR_VERSION=${GRAPHVIZ_MINOR_VERSION})
endif()

set_package_properties(KDSME PROPERTIES URL "https://github.com/KDAB/KDStateMachineEditor" DESCRIPTION "KDAB State Machine Editor framework" TYPE OPTIONAL PURPOSE "Provide Next-gen graphical state machine viewer for the statmeachineviewer plugin.")
if (Qt5_FOUND)
  find_package(KDSME CONFIG QUIET)
endif()

# UI part
if(GAMMARAY_BUILD_UI AND (GRAPHVIZ_FOUND OR KDSME_FOUND))

set(gammaray_statemachineviewer_ui_plugin_libs
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}

  gammaray_common
  gammaray_ui
)

set(gammaray_statemachineviewer_ui_plugin_srcs
  statemachineviewerclient.cpp
  statemachineview.cpp
)

if (KDSME_FOUND AND Qt5Quick_FOUND)
  # new KDSME varaint
  list(APPEND gammaray_statemachineviewer_ui_plugin_srcs
    statemachineviewerwidgetng.cpp
  )
  list(APPEND gammaray_statemachineviewer_ui_plugin_libs
    KDSME::Core
    KDSME::View
  )
else()
  # old Graphviz-based variant
  list(APPEND gammaray_statemachineviewer_ui_plugin_srcs
    gvgraph/gvgraph.cpp
    gvgraph/gvgraphitems.cpp
    gvgraph/gvutils.cpp

    statemachineviewerwidget.cpp
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
  add_definitions(${GRAPHVIZ_COMPILE_FLAGS})

  add_executable(statemachineviewer_test
    gvgraph/gvgraph.cpp
    gvgraph/gvgraphitems.cpp
    gvgraph/gvutils.cpp

    test_main.cpp
  )

  target_link_libraries(statemachineviewer_test ${gammaray_statemachineviewer_ui_plugin_libs})
endif()

qt4_wrap_ui(gammaray_statemachineviewer_ui_plugin_srcs statemachineviewer.ui)

gammaray_add_plugin(gammaray_statemachineviewer_ui_plugin
  gammaray_statemachineviewer_ui.desktop
  ${gammaray_statemachineviewer_shared_srcs}
  ${gammaray_statemachineviewer_ui_plugin_srcs}
)

target_link_libraries(gammaray_statemachineviewer_ui_plugin
  ${gammaray_statemachineviewer_ui_plugin_libs}
)

endif()
