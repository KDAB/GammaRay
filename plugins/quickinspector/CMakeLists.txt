# shared stuff
add_library(gammaray_quickinspector_shared STATIC quickinspectorinterface.cpp)
target_link_libraries(gammaray_quickinspector_shared gammaray_common)
set_target_properties(gammaray_quickinspector_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)

# probe plugin
if(Qt5Quick_FOUND)
  set(gammaray_quickinspector_srcs
    quickinspector.cpp
    quickitemmodel.cpp
  )

  gammaray_add_plugin(gammaray_quickinspector gammaray_quickinspector.desktop ${gammaray_quickinspector_srcs})

  target_link_libraries(gammaray_quickinspector
    gammaray_quickinspector_shared
    gammaray_core Qt5::Quick
  )
endif()


# ui plugin
if(HAVE_QT_WIDGETS)
  set(gammaray_quickinspector_ui_srcs
    quickinspectorclient.cpp
    quickinspectorwidget.cpp
    quickclientitemmodel.cpp
  )

  qt4_wrap_ui(gammaray_quickinspector_ui_srcs
    quickinspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_quickinspector_ui
    gammaray_quickinspector_ui.desktop
    ${gammaray_quickinspector_ui_srcs}
  )

  target_link_libraries(gammaray_quickinspector_ui
    gammaray_quickinspector_shared
    gammaray_ui
  )
endif()
