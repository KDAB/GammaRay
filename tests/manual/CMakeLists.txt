include_directories(
  ${CMAKE_SOURCE_DIR}

  ${CMAKE_CURRENT_BINARY_DIR}
)

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_executable(propertywidgettest
    propertywidgettest.cpp
    )

  target_link_libraries(propertywidgettest
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    gammaray_ui
    gammaray_core
    gammaray_common
  )

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

if(QT_QTWEBKIT_FOUND)
  add_executable(wk1application wk1application.cpp)
  target_link_libraries(wk1application
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTWEBKIT_LIBRARIES}
    ${QT_QTWEBKITWIDGETS_LIBRARIES}
  )
endif()

if(Qt5WebKit_FOUND)
  set(wk2application_srcs wk2application.cpp)
  qt4_add_resources(wk2application_srcs wk2application.qrc)
  add_executable(wk2application ${wk2application_srcs})
  target_link_libraries(wk2application Qt5::Quick)
endif()

if(QNXNTO)
  target_link_libraries(propertywidgettest cpp)
  target_link_libraries(messagemodeltest cpp)
endif()

if(Qt5Widgets_FOUND AND NOT Qt5Widgets_VERSION VERSION_LESS 5.2.0)
  add_executable(variantinspectortest
    variantinspector.cpp
    variantinspectorapplication.cpp
  )
  set_property(TARGET variantinspectortest PROPERTY AUTOMOC TRUE)
  target_link_libraries(variantinspectortest Qt5::Widgets)
endif()
