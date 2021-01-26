if(TARGET Qt::Widgets)
  set(gammaray_messagemodeltest_srcs
    messagemodeltest.cpp
  )

  add_executable(messagemodeltest
    ${gammaray_messagemodeltest_srcs}
  )

  target_link_libraries(messagemodeltest
    Qt::Core
    Qt::Gui Qt::Widgets
  )

  add_executable(modelstest modelstest.cpp)
  target_link_libraries(modelstest Qt::Gui Qt::Widgets gammaray_shared_test_data)
endif()

add_executable(propertytest propertytest.cpp)
target_link_libraries(propertytest Qt::Core gammaray_shared_test_data)

add_executable(connectionstest connectionstest.cpp)
target_link_libraries(connectionstest Qt::Core)

if(TARGET Qt::Widgets)
  add_executable(translator_test translator_test.cpp)
  target_link_libraries(translator_test Qt::Widgets)
  add_custom_target(translator_test_qm
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/translation.qm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translation.qm
  )
  add_dependencies(translator_test translator_test_qm)
endif()

if(Qt5Webkit_FOUND)
  add_executable(wk1application wk1application.cpp)
  target_link_libraries(wk1application
    Qt::Core
    Qt::Gui Qt::Widgets
    Qt5::Webkit
    Qt5::WebkitWidgets
  )
endif()

if(Qt5WebKit_FOUND AND TARGET Qt::Quick)
  set(wk2application_srcs wk2application.cpp wk2application.qrc)
  add_executable(wk2application ${wk2application_srcs})
  target_link_libraries(wk2application Qt::Quick)
endif()

add_executable(signalmonitortest signalmonitortest.cpp)

target_link_libraries(signalmonitortest
  Qt::Core
  Qt::Gui
)

add_executable(objectreparenttest objectreparenttest.cpp)
target_link_libraries(objectreparenttest Qt::Core)

if(TARGET Qt::QuickWidgets)
  set(quickwidgettest_srcs quickwidgettest.cpp quickwidgettest.qrc)
  add_executable(quickwidgettest ${quickwidgettest_srcs})
  target_link_libraries(quickwidgettest Qt::QuickWidgets)
endif()

if(GAMMARAY_STATIC_PROBE)
  add_executable(staticinjectiontest staticinjectiontest.cpp)
  target_link_libraries(staticinjectiontest
    gammaray_probe
    Qt::Widgets
    gammaray_actioninspector_plugin
    gammaray_codecbrowser_plugin
    gammaray_eventmonitor_plugin
    gammaray_guisupport
    gammaray_fontbrowser_plugin
    gammaray_sceneinspector_plugin
    gammaray_signalmonitor
    gammaray_statemachineviewer_plugin
    gammaray_styleinspector_plugin
    gammaray_timertop_plugin
    gammaray_translatorinspector
    gammaray_widgetinspector_plugin

#    gammaray_bluetooth
#    gammaray_qmlsupport
#    gammaray_quickinspector
#    gammaray_webinspector_plugin
  )
endif()
