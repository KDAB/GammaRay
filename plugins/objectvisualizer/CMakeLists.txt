# VTK includes QtGui/... stuff => breaks compilation with Qt5
if(NOT Qt5Core_FOUND)

if(VTK_FOUND)

add_definitions(-DHAVE_VTK -DVTK_EXCLUDE_STRSTREAM_HEADERS)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${VTK_INCLUDE_DIRS}
)

include(${VTK_USE_FILE})

link_directories(${VTK_LIBRARY_DIRS})

set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  objectvisualizerwidget.cpp
  vtkcontainer.cpp
  vtkpanel.cpp
  vtkwidget.cpp
)

qt4_automoc(${gammaray_objectvisualizer_plugin_srcs})

gammaray_add_plugin(gammaray_objectvisualizer_plugin
  gammaray_objectvisualizer.desktop
  ${gammaray_objectvisualizer_plugin_srcs}
)

if (${VTK_MAJOR_VERSION} EQUAL 5)
  set(VTK_QT_LIBRARY QVTK)
elseif (${VTK_MAJOR_VERSION} EQUAL 6)
  set(VTK_QT_LIBRARY vtkGUISupportQt)
endif ()

target_link_libraries(gammaray_objectvisualizer_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${VTK_LIBRARIES}
  ${VTK_QT_LIBRARY}
  gammaray_core
)

endif()

endif()
