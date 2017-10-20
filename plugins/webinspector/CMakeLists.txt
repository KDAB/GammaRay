# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
    set(gammaray_webinspector_plugin_srcs
        webinspector.cpp
        webviewmodel.cpp
    )

    gammaray_add_plugin(gammaray_webinspector_plugin
        JSON gammaray_webinspector.json
        SOURCES ${gammaray_webinspector_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_plugin
        gammaray_core
    )
endif()

# ui part
if(GAMMARAY_BUILD_UI AND TARGET Qt5::WebEngineWidgets)
    set(gammaray_webinspector_ui_plugin_srcs
        webinspectorwidget.cpp
    )

    gammaray_add_plugin(gammaray_webinspector_ui_plugin
        JSON gammaray_webinspector.json
        SOURCES ${gammaray_webinspector_ui_plugin_srcs}
    )

    target_link_libraries(gammaray_webinspector_ui_plugin
        Qt5::WebEngineWidgets
        gammaray_ui
    )
endif()
