set(gammaray_clientlib_srcs
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

qt4_add_resources(gammaray_clientlib_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)
add_library(gammaray_client SHARED ${gammaray_clientlib_srcs})
set_target_properties(gammaray_client PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  DEFINE_SYMBOL MAKE_GAMMARAY_CLIENT_LIB
)

target_link_libraries(gammaray_client
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

add_executable(gammaray-client WIN32 ${gammaray_client_srcs})

target_link_libraries(gammaray-client
  gammaray_client
  gammaray_common
  ${QT_QTGUI_LIBRARIES}
)

if(QNXNTO)
  target_link_libraries(gammaray-client cpp)
endif()

gammaray_embed_info_plist(gammaray-client ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
set_target_properties(gammaray-client PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${LIBEXEC_INSTALL_DIR}"
)

install(TARGETS gammaray_client EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS gammaray-client DESTINATION ${LIBEXEC_INSTALL_DIR})


gammaray_install_headers(
  gammaray_client_export.h
  clientconnectionmanager.h
)

ecm_generate_pri_file(BASE_NAME GammaRayClient
                      LIB_NAME gammaray_client
                      DEPS "core gui widgets network GammaRayCommon GammaRayUi"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
