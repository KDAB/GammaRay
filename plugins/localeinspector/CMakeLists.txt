# probe part
set(gammaray_localeinspector_plugin_srcs
    localeinspector.cpp
    localemodel.cpp
    localedataaccessor.cpp
    localeaccessormodel.cpp
)
if(NOT Qt5Core_VERSION VERSION_LESS 5.2.0)
    list(APPEND gammaray_localeinspector_plugin_srcs timezonemodel.cpp)
endif()

gammaray_add_plugin(gammaray_localeinspector_plugin
    DESKTOP gammaray_localeinspector.desktop.in
    JSON gammaray_localeinspector.json
    SOURCES ${gammaray_localeinspector_plugin_srcs}
)

target_link_libraries(gammaray_localeinspector_plugin gammaray_core)

# ui part
if(GAMMARAY_BUILD_UI)
    set(gammaray_localeinspector_plugin_ui_srcs
        localeinspectorwidget.cpp
        localetab.cpp
        timezonetab.cpp
        timezoneclientmodel.cpp
    )

    qt4_wrap_ui(gammaray_localeinspector_plugin_ui_srcs
        localeinspectorwidget.ui
        localetab.ui
        timezonetab.ui
    )

    gammaray_add_plugin(gammaray_localeinspector_ui_plugin
        DESKTOP gammaray_localeinspector_ui.desktop.in
        JSON gammaray_localeinspector.json
        SOURCES ${gammaray_localeinspector_plugin_ui_srcs}
    )

    target_link_libraries(gammaray_localeinspector_ui_plugin gammaray_ui)
endif()
