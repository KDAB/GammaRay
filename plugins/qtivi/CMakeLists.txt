# probe plugin
if(Qt5IviCore_FOUND)
    gammaray_add_plugin(gammaray_qtivi JSON gammaray_qtivi.json SOURCES qtivisupport.cpp qtivipropertymodel.cpp
                        qtivipropertyoverrider.cpp)
    target_include_directories(gammaray_qtivi SYSTEM PRIVATE ${Qt5IviCore_PRIVATE_INCLUDE_DIRS})
    target_link_libraries(gammaray_qtivi gammaray_core Qt5::IviCore)
endif()

# ui part
if(GAMMARAY_BUILD_UI)
    gammaray_add_plugin(gammaray_qtivi_ui JSON gammaray_qtivi.json SOURCES qtivisupportwidget.cpp)
    target_link_libraries(gammaray_qtivi_ui ${QT_QTGUI_LIBRARIES} gammaray_ui)
endif()
