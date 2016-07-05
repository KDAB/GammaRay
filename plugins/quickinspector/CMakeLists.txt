
# check if we can build the Quick inspector
# TODO 5.1.0 and below require private V8 headers which do not provide CMake variables,
# so we would need to find those manually
if(Qt5Quick_FOUND AND HAVE_PRIVATE_Qt5Quick_HEADERS AND NOT ${Qt5Quick_VERSION} VERSION_LESS 5.2.0)
  set(HAVE_QUICK_INSPECTOR TRUE)

  # shared stuff
  set(gammaray_quickinspector_shared_srcs
        quickinspectorinterface.cpp
        quickitemgeometry.cpp
        materialextension/materialextensioninterface.cpp
      )

  add_library(gammaray_quickinspector_shared STATIC ${gammaray_quickinspector_shared_srcs})
  target_link_libraries(gammaray_quickinspector_shared gammaray_common Qt5::Gui)
  set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

  include_directories(SYSTEM ${Qt5Quick_PRIVATE_INCLUDE_DIRS})

  set(gammaray_quickinspector_srcs
    quickinspector.cpp
    quickitemmodel.cpp
    quickscenegraphmodel.cpp
    quickpaintanalyzerextension.cpp

    materialextension/materialextension.cpp
    geometryextension/sggeometryextension.cpp
    geometryextension/sggeometrymodel.cpp
  )

  gammaray_add_plugin(gammaray_quickinspector
    JSON gammaray_quickinspector.json
    SOURCES ${gammaray_quickinspector_srcs}
  )

  target_link_libraries(gammaray_quickinspector
    gammaray_quickinspector_shared
    gammaray_core Qt5::Quick
    gammaray_kitemmodels
  )

  if(GAMMARAY_BUILD_UI)
    # ui plugin
    set(gammaray_quickinspector_ui_srcs
      quickinspectorwidget.cpp
      quickinspectorclient.cpp
      quickclientitemmodel.cpp
      quickitemdelegate.cpp
      quickitemtreewatcher.cpp
      quickscenepreviewwidget.cpp

      materialextension/materialextensionclient.cpp
      materialextension/materialtab.cpp
      geometryextension/sggeometrytab.cpp
      geometryextension/sgwireframewidget.cpp
    )

    qt4_add_resources(gammaray_quickinspector_ui_srcs
      resources/quickinspector.qrc
    )

    qt4_wrap_ui(gammaray_quickinspector_ui_srcs
      quickinspectorwidget.ui
      materialextension/materialtab.ui
      geometryextension/sggeometrytab.ui
    )

    gammaray_add_plugin(gammaray_quickinspector_ui
      JSON gammaray_quickinspector.json
      SOURCES ${gammaray_quickinspector_ui_srcs}
    )

    target_include_directories(gammaray_quickinspector_ui PRIVATE ${CMAKE_BINARY_DIR}/client) # for remotemodel.h
    target_link_libraries(gammaray_quickinspector_ui
      gammaray_quickinspector_shared
      gammaray_ui
      Qt5::Quick
    )
  endif()
endif()

add_feature_info(
  "QtQuick 2 inspector"
  HAVE_QUICK_INSPECTOR
  "Requires private Qt5Quick headers and Qt >= 5.2.0 to be available."
)
