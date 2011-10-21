include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}

  ${VTK_INCLUDE_DIRS}
)

link_directories(${VTK_LIBRARY_DIRS})

set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  vtkcontainer.cpp
  vtkpanel.cpp
  vtkwidget.cpp

  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kfilterproxysearchline.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/krecursivefilterproxymodel.cpp
  ${CMAKE_SOURCE_DIR}/util.cpp
)
qt4_automoc(${gammaray_objectvisualizer_plugin_srcs})

add_library(gammaray_objectvisualizer_plugin MODULE ${gammaray_objectvisualizer_plugin_srcs})
target_link_libraries(gammaray_objectvisualizer_plugin
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  QVTK
)
set_target_properties(gammaray_objectvisualizer_plugin
  PROPERTIES PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/plugins
)
install(
  TARGETS gammaray_objectvisualizer_plugin
  DESTINATION ${PLUGIN_INSTALL_DIR}/gammaray/
)
