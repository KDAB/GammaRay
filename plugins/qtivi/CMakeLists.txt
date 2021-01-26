# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
if(TARGET Qt::IviCore)
    set(plugin_files qtivisupport.cpp)
    if(QtIviCore_VERSION VERSION_GREATER 1.1)
        add_definitions(-DQTIVI_DISABLE_IVIPROPERTIES)
        set(plugin_files ${plugin_files} qtiviobjectmodel.cpp)
    else()
        set(plugin_files ${plugin_files} qtivipropertymodel.cpp qtivipropertyoverrider.cpp)
    endif()
    gammaray_add_plugin(gammaray_qtivi JSON gammaray_qtivi.json SOURCES ${plugin_files})
    target_link_libraries(gammaray_qtivi
        gammaray_core
        Qt::IviCore
        Qt::IviCorePrivate
    )
    if(TARGET Qt::IviVehicleFunctions)
        target_link_libraries(gammaray_qtivi Qt::IviVehicleFunctions)
    endif()
    if(TARGET QtIviMedia)
        target_link_libraries(gammaray_qtivi Qt::IviMedia)
    endif()
endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    gammaray_add_plugin(gammaray_qtivi_ui
        JSON
            gammaray_qtivi.json
        SOURCES
            qtivisupportwidget.cpp
            qtiviconstrainedvaluedelegate.cpp
            qtivipropertyclientmodel.cpp
    )
    target_link_libraries(gammaray_qtivi_ui gammaray_ui)
endif()
