# shared stuff
set(gammaray_quickinspector_shared_srcs
      quickinspectorinterface.cpp
      materialextension/materialextensioninterface.cpp
      geometryextension/sggeometryextensioninterface.cpp
    )

add_library(gammaray_quickinspector_shared STATIC ${gammaray_quickinspector_shared_srcs})
target_link_libraries(gammaray_quickinspector_shared gammaray_common)
set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

# probe plugin
if(Qt5Quick_FOUND)

  # check if we can build the QSG stuff
  # TODO 5.1.0 and below require private V8 headers which do not provide CMake variables,
  # so we would need to find those manually
  if (Qt5Quick_PRIVATE_INCLUDE_DIRS AND NOT ${Qt5Quick_VERSION} VERSION_LESS 5.2.0)
    set(HAVE_SG_INSPECTOR TRUE)
    add_definitions(-DHAVE_SG_INSPECTOR)
  endif()
  add_feature_info("QSG inspector" HAVE_SG_INSPECTOR "Requires private Qt headers and Qt >= 5.2.0 to be available.")

  set(gammaray_quickinspector_srcs
    quickinspector.cpp
    quickitemmodel.cpp

    materialextension/materialextension.cpp
    geometryextension/sggeometryextension.cpp
    geometryextension/sggeometrymodel.cpp
  )

  if(HAVE_SG_INSPECTOR)
    include_directories(${Qt5Quick_PRIVATE_INCLUDE_DIRS})

    set(gammaray_quickinspector_srcs
      ${gammaray_quickinspector_srcs}
      quickscenegraphmodel.cpp
    )
  endif()

  gammaray_add_plugin(gammaray_quickinspector gammaray_quickinspector.desktop ${gammaray_quickinspector_srcs})

  target_link_libraries(gammaray_quickinspector
    gammaray_quickinspector_shared
    gammaray_core Qt5::Quick
  )

  # ui plugin
  set(gammaray_quickinspector_ui_srcs
    quickinspectorclient.cpp
    quickinspectorwidget.cpp
    quickclientitemmodel.cpp
    quickitemdelegatewidget.cpp
    quickitemtreewatcher.cpp
    quickitemoverlay.cpp

    materialextension/materialextensionclient.cpp
    materialextension/materialtab.cpp
    geometryextension/sggeometryextensionclient.cpp
    geometryextension/sggeometrytab.cpp
    geometryextension/sgwireframewidget.cpp
  )
  qt4_add_resources(gammaray_quickinspector_ui_srcs resources/quickinspector.qrc)

  qt4_wrap_ui(gammaray_quickinspector_ui_srcs
    quickinspectorwidget.ui
    materialextension/materialtab.ui
    geometryextension/sggeometrytab.ui
  )

  gammaray_add_plugin(gammaray_quickinspector_ui
    gammaray_quickinspector_ui.desktop
    ${gammaray_quickinspector_ui_srcs}
  )

  target_link_libraries(gammaray_quickinspector_ui
    gammaray_quickinspector_shared
    gammaray_ui
    Qt5::Quick
  )
endif()
