include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_client_srcs
  main.cpp
  client.cpp
  remotemodel.cpp
  selectionmodelclient.cpp
  clientconnectionmanager.cpp
  propertycontrollerclient.cpp
  probecontrollerclient.cpp
)

qt4_add_resources(gammaray_client_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_executable(gammaray-client WIN32 ${gammaray_client_srcs})

target_link_libraries(gammaray-client
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTNETWORK_LIBRARIES}
  gammaray_common
  gammaray_ui
  gammaray_ui_internal
)

if(QNXNTO)
  target_link_libraries(gammaray-client cpp)
endif()

install(TARGETS gammaray-client DESTINATION ${LIBEXEC_INSTALL_DIR})
