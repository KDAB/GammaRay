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
endif()

add_executable(minimalcoreapplication minimalcoreapplication.cpp)
target_link_libraries(minimalcoreapplication ${QT_QTCORE_LIBRARIES})

add_executable(propertytest propertytest.cpp)
target_link_libraries(propertytest ${QT_QTCORE_LIBRARIES} gammaray_shared_test_data)

add_executable(connectionstest connectionstest.cpp)
target_link_libraries(connectionstest ${QT_QTCORE_LIBRARIES})

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
  set(wk2application_srcs wk2application.cpp)
  qt4_add_resources(wk2application_srcs wk2application.qrc)
  add_executable(wk2application ${wk2application_srcs})
  target_link_libraries(wk2application Qt5::Quick)
endif()

add_executable(signalmonitortest signalmonitortest.cpp)

target_link_libraries(signalmonitortest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
)

if(QNXNTO)
  target_link_libraries(messagemodeltest cpp)
endif()

add_executable(objectreparenttest objectreparenttest.cpp)
target_link_libraries(objectreparenttest ${QT_QTCORE_LIBRARIES})
