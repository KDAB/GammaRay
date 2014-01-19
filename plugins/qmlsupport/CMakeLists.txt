# probe plugin
gammaray_add_plugin(gammaray_qmlsupport gammaray_qmlsupport.desktop qmlsupport.cpp)
target_link_libraries(gammaray_qmlsupport gammaray_core Qt5::Qml)
