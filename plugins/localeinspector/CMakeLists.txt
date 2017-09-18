# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_localeinspector_plugin_srcs
    localeinspector.cpp
    localemodel.cpp
    localedataaccessor.cpp
    localeaccessormodel.cpp
)
if(NOT Qt5Core_VERSION VERSION_LESS 5.2.0)
    list(APPEND gammaray_localeinspector_plugin_srcs
        timezonemodel.cpp
        timezoneoffsetdatamodel.cpp
    )
endif()

gammaray_add_plugin(gammaray_localeinspector_plugin
    DESKTOP gammaray_localeinspector.desktop.in
    JSON gammaray_localeinspector.json
    SOURCES ${gammaray_localeinspector_plugin_srcs}
)

target_link_libraries(gammaray_localeinspector_plugin gammaray_core)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_localeinspector_plugin_ui_srcs
        localeinspectorwidget.cpp
        localetab.cpp
        timezonetab.cpp
        timezoneclientmodel.cpp
        timezoneoffsetdataclientmodel.cpp
    )

    gammaray_add_plugin(gammaray_localeinspector_ui_plugin
        DESKTOP gammaray_localeinspector_ui.desktop.in
        JSON gammaray_localeinspector.json
        SOURCES ${gammaray_localeinspector_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_localeinspector_ui_plugin gammaray_ui)
endif()
