include_directories(
  ${CMAKE_SOURCE_DIR}

  ${CMAKE_CURRENT_BINARY_DIR}
)

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

qt4_automoc(${gammaray_messagemodeltest_srcs})

target_link_libraries(messagemodeltest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
)

add_executable(minimalcoreapplication minimalcoreapplication.cpp)
target_link_libraries(minimalcoreapplication ${QT_QTCORE_LIBRARIES})

if(QNXNTO)
  target_link_libraries(propertywidgettest cpp)
  target_link_libraries(messagemodeltest cpp)
endif()
