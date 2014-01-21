# probe plugin
if(Qt5Quick_FOUND)
  gammaray_add_plugin(gammaray_quickinspector gammaray_quickinspector.desktop quickinspector.cpp)
  target_link_libraries(gammaray_quickinspector gammaray_core Qt5::Quick)
endif()


# ui plugin
if(HAVE_QT_WIDGETS)
  set(gammaray_quickinspector_ui_srcs
    quickinspectorwidget.cpp
  )

  qt4_wrap_ui(gammaray_quickinspector_ui_srcs
    quickinspectorwidget.ui
  )

  gammaray_add_plugin(gammaray_quickinspector_ui
    gammaray_quickinspector_ui.desktop
    ${gammaray_quickinspector_ui_srcs}
  )

  target_link_libraries(gammaray_quickinspector_ui
    ${QT_GUI_LIBRARIES}
    gammaray_ui
  )
endif()
