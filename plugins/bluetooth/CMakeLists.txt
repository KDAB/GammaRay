# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
gammaray_add_plugin(gammaray_bluetooth JSON gammaray_bluetooth.json SOURCES bluetooth.cpp)
target_link_libraries(gammaray_bluetooth gammaray_core Qt::Bluetooth)
endif()
