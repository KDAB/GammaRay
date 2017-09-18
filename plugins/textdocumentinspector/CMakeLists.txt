# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_textdocumentinspector_srcs
    textdocumentinspector.cpp
    textdocumentformatmodel.cpp
    textdocumentmodel.cpp
)
gammaray_add_plugin(gammaray_textdocumentinspector
    JSON gammaray_textdocumentinspector.json
    DESKTOP gammaray_textdocumentinspector.desktop
    SOURCES ${gammaray_textdocumentinspector_srcs}
)
target_link_libraries(gammaray_textdocumentinspector gammaray_core)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_textdocumentinspector Qt5::Gui)
else()
  target_link_libraries(gammaray_textdocumentinspector ${QT_GUI_LIBRARIES})
endif()
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
    set(gammaray_textdocumentinspector_ui_srcs
        textdocumentinspectorwidget.cpp
        textdocumentcontentview.cpp
    )

    gammaray_add_plugin(gammaray_textdocumentinspector_ui
        JSON gammaray_textdocumentinspector.json
        DESKTOP gammaray_textdocumentinspector_ui.desktop
        SOURCES ${gammaray_textdocumentinspector_ui_srcs}
    )
    target_link_libraries(gammaray_textdocumentinspector_ui gammaray_ui)
endif()
