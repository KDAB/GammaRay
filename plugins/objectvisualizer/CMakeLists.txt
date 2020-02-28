# probe part
set(gammaray_objectvisualizer_plugin_srcs
  objectvisualizer.cpp
  objectvisualizermodel.cpp
)

gammaray_add_plugin(gammaray_objectvisualizer_plugin
  JSON gammaray_objectvisualizer.json
  SOURCES ${gammaray_objectvisualizer_plugin_srcs}
)

target_link_libraries(gammaray_objectvisualizer_plugin
  gammaray_kitemmodels
  gammaray_core
)

######## START VTK VISUALIZATION

find_package(VTK)
set_package_properties(VTK PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Visualization Toolkit"
  PURPOSE "Needed for the object visualizer plugin (required Qt5). Just for toying around, highly experimental plugin."
  URL "https://www.vtk.org"
)
if(VTK_FOUND)
  find_path(VTK_QT_INCLUDE_DIR NAMES QVTKWidget.h HINTS ${VTK_INCLUDE_DIRS})
  if(NOT VTK_QT_INCLUDE_DIR)
    message(STATUS "Cannot locate QVTKWidget.h in ${VTK_INCLUDE_DIRS}")
    set(VTK_FOUND FALSE)
    add_feature_info("QVTKWidget header" VTK_FOUND "Object visualizer plugin requires QVTKWidget.h header.")
  endif()
endif()

# Only possibly build the plugin under Qt5 (and when VTK was built with Qt5 support as well)
if(VTK_FOUND AND VTK_QT_VERSION EQUAL 5 AND GAMMARAY_BUILD_UI)
  set(OPTIONAL_VTK_LIBRARIES)

  add_definitions(-DVTKGUI_ENABLED -DVTK_EXCLUDE_STRSTREAM_HEADERS)

  include(${VTK_USE_FILE})

  set(gammaray_objectvisualizer_ui_plugin_srcs
    objectvisualizerwidget.cpp
    vtkcontainer.cpp
    vtkpanel.cpp
    vtkwidget.cpp
  )

  gammaray_add_plugin(gammaray_objectvisualizer_ui_plugin
    JSON gammaray_objectvisualizer.json
    SOURCES ${gammaray_objectvisualizer_ui_plugin_srcs}
  )
  target_include_directories(gammaray_objectvisualizer_ui_plugin SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
  target_link_libraries(gammaray_objectvisualizer_ui_plugin
    ${VTK_LIBRARIES}
    gammaray_common
    gammaray_ui
  )
endif()

######## END VTK VISUALIZATION
