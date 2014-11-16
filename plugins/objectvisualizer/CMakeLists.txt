# probe part
set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  objectvisualizermodel.cpp
)

gammaray_add_plugin(gammaray_objectvisualizer_plugin
  gammaray_objectvisualizer.desktop
  ${gammaray_objectvisualizer_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_plugin
  ${QT_QTCORE_LIBRARIES}
  gammaray_core
)

######## START VTK VISUALIZATION
# VTK includes QtGui/... stuff => breaks compilation with Qt5
if(VTK_FOUND AND NOT Qt5Core_FOUND AND GAMMARAY_BUILD_UI)

set(OPTIONAL_VTK_LIBRARIES
)

add_definitions(-DVTKGUI_ENABLED -DVTK_EXCLUDE_STRSTREAM_HEADERS)

include_directories(
  ${VTK_INCLUDE_DIRS}
)

include(${VTK_USE_FILE})

link_directories(${VTK_LIBRARY_DIRS})

set(gammaray_objectvisualizer_ui_plugin_srcs
  objectvisualizerwidget.cpp
  vtkcontainer.cpp
  vtkpanel.cpp
  vtkwidget.cpp
)

list(APPEND OPTIONAL_VTK_LIBRARIES
  ${VTK_LIBRARIES}
)

if(${VTK_MAJOR_VERSION} EQUAL 5)
  list(APPEND OPTIONAL_VTK_LIBRARIES QVTK)
elseif(${VTK_MAJOR_VERSION} EQUAL 6)
  list(APPEND OPTIONAL_VTK_LIBRARIES vtkGUISupportQt)
endif()

gammaray_add_plugin(gammaray_objectvisualizer_ui_plugin
  gammaray_objectvisualizer_ui.desktop
  ${gammaray_objectvisualizer_ui_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_ui_plugin
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${OPTIONAL_VTK_LIBRARIES}
  ${VTK_QT_LIBRARY}
  gammaray_common
  gammaray_ui
)

endif()

######## END VTK VISUALIZATION
