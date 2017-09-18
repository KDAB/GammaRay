# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_mimetypes_srcs
    mimetypes.cpp
    mimetypesmodel.cpp
)
gammaray_add_plugin(gammaray_mimetypes JSON gammaray_mimetypes.json SOURCES ${gammaray_mimetypes_srcs})
target_link_libraries(gammaray_mimetypes gammaray_core Qt5::Gui gammaray_kitemmodels)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_mimetypes_ui_srcs
    mimetypeswidget.cpp
  )
  gammaray_add_plugin(gammaray_mimetypes_ui JSON gammaray_mimetypes.json SOURCES ${gammaray_mimetypes_ui_srcs})
  target_link_libraries(gammaray_mimetypes_ui gammaray_ui)
endif()
