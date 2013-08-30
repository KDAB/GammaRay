include_directories(
  ${CMAKE_SOURCE_DIR}

  ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(propertywidgettest propertywidgettest.cpp)

target_link_libraries(propertywidgettest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_ui
  gammaray_core
  gammaray_common
)

qt4_automoc(signalmonitortest.cpp)
add_executable(signalmonitortest signalmonitortest.cpp)

target_link_libraries(signalmonitortest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_ui
  gammaray_core
  gammaray_common
)

if(QNXNTO)
  target_link_libraries(propertywidgettest cpp)
endif()
