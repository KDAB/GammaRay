if(VTK_FOUND)

add_definitions(-DHAVE_VTK)

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

gammaray_add_plugin(gammaray_objectvisualizer_plugin gammaray_objectvisualizer.desktop ${gammaray_objectvisualizer_plugin_srcs})
target_link_libraries(gammaray_objectvisualizer_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  QVTK
)

endif()

