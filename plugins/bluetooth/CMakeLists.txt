# probe plugin
gammaray_add_plugin(gammaray_bluetooth gammaray_bluetooth.desktop bluetooth.cpp)
target_link_libraries(gammaray_bluetooth gammaray_core Qt5::Bluetooth)
