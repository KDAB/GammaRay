set(gammaray_client_internal_srcs
  client.cpp
  remotemodel.cpp
  selectionmodelclient.cpp
  clientconnectionmanager.cpp
  propertycontrollerclient.cpp
  probecontrollerclient.cpp
  clientdevice.cpp
  tcpclientdevice.cpp
  localclientdevice.cpp
)

add_library(gammaray_client_internal STATIC ${gammaray_client_internal_srcs})
target_link_libraries(gammaray_client_internal
  LINK_PUBLIC
  ${QT_QTCORE_LIBRARIES}

  LINK_PRIVATE
  gammaray_ui
  gammaray_ui_internal
  gammaray_common
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTNETWORK_LIBRARIES}
)


set(gammaray_client_srcs main.cpp)
qt4_add_resources(gammaray_client_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_executable(gammaray-client WIN32 ${gammaray_client_srcs})

target_link_libraries(gammaray-client
  gammaray_client_internal
  gammaray_common
)

if(QNXNTO)
  target_link_libraries(gammaray-client cpp)
endif()

gammaray_embed_info_plist(gammaray-client ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
set_target_properties(gammaray-client PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${LIBEXEC_INSTALL_DIR}"
)

install(TARGETS gammaray-client DESTINATION ${LIBEXEC_INSTALL_DIR})
