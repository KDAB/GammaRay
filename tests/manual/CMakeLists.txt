if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  set(gammaray_messagemodeltest_srcs
    messagemodeltest.cpp
  )

  add_executable(messagemodeltest
    ${gammaray_messagemodeltest_srcs}
  )

  target_link_libraries(messagemodeltest
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
  )

  add_executable(modelstest modelstest.cpp)
  target_link_libraries(modelstest ${QT_QTGUI_LIBRARIES} gammaray_shared_test_data)
endif()

add_executable(propertytest propertytest.cpp)
target_link_libraries(propertytest ${QT_QTCORE_LIBRARIES} gammaray_shared_test_data)

add_executable(connectionstest connectionstest.cpp)
target_link_libraries(connectionstest ${QT_QTCORE_LIBRARIES})

if(Qt5Widgets_FOUND)
  add_executable(translator_test translator_test.cpp)
  target_link_libraries(translator_test Qt5::Widgets)
  add_custom_target(translator_test_qm
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/translation.qm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translation.qm
  )
  add_dependencies(translator_test translator_test_qm)
endif()

if(QT_QTWEBKIT_FOUND)
  add_executable(wk1application wk1application.cpp)
  target_link_libraries(wk1application
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTWEBKIT_LIBRARIES}
    ${QT_QTWEBKITWIDGETS_LIBRARIES}
  )
endif()

if(Qt5WebKit_FOUND AND Qt5Quick_FOUND)
  set(wk2application_srcs wk2application.cpp wk2application.qrc)
  add_executable(wk2application ${wk2application_srcs})
  target_link_libraries(wk2application Qt5::Quick)
endif()

add_executable(signalmonitortest signalmonitortest.cpp)

target_link_libraries(signalmonitortest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
)

add_executable(objectreparenttest objectreparenttest.cpp)
target_link_libraries(objectreparenttest ${QT_QTCORE_LIBRARIES})

if(Qt5QuickWidgets_FOUND)
  set(quickwidgettest_srcs quickwidgettest.cpp quickwidgettest.qrc)
  add_executable(quickwidgettest ${quickwidgettest_srcs})
  target_link_libraries(quickwidgettest Qt5::QuickWidgets)
endif()

if(GAMMARAY_STATIC_PROBE)
  add_executable(staticinjectiontest staticinjectiontest.cpp)
  target_link_libraries(staticinjectiontest
    gammaray_probe
    Qt5::Widgets
    gammaray_actioninspector_plugin
    gammaray_codecbrowser_plugin
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
