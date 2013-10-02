include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(OPTIONAL_VTK_LIBRARIES
)

set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  objectvisualizermodel.cpp
)

######## START VTK VISUALIZATION
# VTK includes QtGui/... stuff => breaks compilation with Qt5
if(NOT Qt5Core_FOUND)

if(VTK_FOUND)

add_definitions(-DVTKGUI_ENABLED -DVTK_EXCLUDE_STRSTREAM_HEADERS)

include_directories(
  ${VTK_INCLUDE_DIRS}
)

include(${VTK_USE_FILE})

link_directories(${VTK_LIBRARY_DIRS})

LIST(APPEND gammaray_objectvisualizer_plugin_srcs
  objectvisualizerwidget.cpp
  vtkcontainer.cpp
  vtkpanel.cpp
  vtkwidget.cpp
)

LIST(APPEND OPTIONAL_VTK_LIBRARIES
  ${VTK_LIBRARIES}
)

if (${VTK_MAJOR_VERSION} EQUAL 5)
  LIST(APPEND OPTIONAL_VTK_LIBRARIES QVTK)
elseif (${VTK_MAJOR_VERSION} EQUAL 6)
  LIST(APPEND OPTIONAL_VTK_LIBRARIES vtkGUISupportQt)
endif ()

endif()

endif()
######## END VTK VISUALIZATION

qt4_automoc(${gammaray_objectvisualizer_plugin_srcs})

gammaray_add_plugin(gammaray_objectvisualizer_plugin
  gammaray_objectvisualizer.desktop
  ${gammaray_objectvisualizer_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${OPTIONAL_VTK_LIBRARIES}
  ${VTK_QT_LIBRARY}
  gammaray_core
)
