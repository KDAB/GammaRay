gammaray_add_plugin(gammaray_translatorinspector gammaray_translatorinspector.desktop
  translatorinspector.cpp
  translatorinspectorinterface.cpp
  translatorwrapper.cpp
  translatorsmodel.cpp
)
target_link_libraries(gammaray_translatorinspector gammaray_core Qt5::Core Qt5::Gui)

qt5_wrap_ui(translatorinspector_ui_srcs
  translatorinspectorwidget.ui
)
gammaray_add_plugin(gammaray_translatorinspector_ui gammaray_translatorinspector_ui.desktop translatorinspectorwidget.cpp translatorinspectorinterface.cpp ${translatorinspector_ui_srcs})
target_link_libraries(gammaray_translatorinspector_ui gammaray_ui)

add_executable(translator_test main.cpp)
qt5_use_modules(translator_test Widgets)

add_custom_target(translator_test_qm
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/translation.qm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translation.qm
)
add_dependencies(translator_test translator_test_qm)
