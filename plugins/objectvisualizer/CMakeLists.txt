# probe part
set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  objectvisualizermodel.cpp
)

gammaray_add_plugin(gammaray_objectvisualizer_plugin
  DESKTOP gammaray_objectvisualizer.desktop.in
  SOURCES ${gammaray_objectvisualizer_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_plugin
  gammaray_kitemmodels
  gammaray_core
)

######## START VTK VISUALIZATION

#VTK discovery works a lot better if you give CMake a hint using the VTK_DIR variable
find_path(VTK_DIR VTKConfig.cmake
  /usr/lib64/vtk /usr/lib/vtk /usr/local/lib64/vtk /usr/local/lib/vtk
)
find_package(VTK QUIET)
set_package_properties(VTK PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Visualization Toolkit"
  PURPOSE "Needed for the object visualizer plugin."
  URL "http://www.vtk.org"
)
if(VTK_FOUND)
  find_path(VTK_QT_INCLUDE_DIR NAMES QVTKWidget.h HINTS ${VTK_INCLUDE_DIRS})
  if(NOT VTK_QT_INCLUDE_DIR)
    message(STATUS "Cannot locate QVTKWidget.h in ${VTK_INCLUDE_DIRS}")
    set(VTK_FOUND FALSE)
    add_feature_info("QVTKWidget header" VTK_FOUND "Object visualizer plugin requires QVTKWidget.h header.")
  endif()
endif()
set(HAVE_VTK ${VTK_FOUND})

# VTK includes QtGui/... stuff => breaks compilation with Qt5
if(VTK_FOUND AND NOT Qt5Core_FOUND AND GAMMARAY_BUILD_UI)

set(OPTIONAL_VTK_LIBRARIES
)

add_definitions(-DVTKGUI_ENABLED -DVTK_EXCLUDE_STRSTREAM_HEADERS)

include_directories(SYSTEM ${VTK_INCLUDE_DIRS})

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
  DESKTOP gammaray_objectvisualizer_ui.desktop.in
  SOURCES ${gammaray_objectvisualizer_ui_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_ui_plugin
  ${OPTIONAL_VTK_LIBRARIES}
  ${VTK_QT_LIBRARY}
  gammaray_common
  gammaray_ui
)

endif()

######## END VTK VISUALIZATION
