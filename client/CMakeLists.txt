include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

set(gammaray_client_srcs
  main.cpp
  client.cpp
  remotemodel.cpp
)

qt4_automoc(${gammaray_client_srcs})
add_executable(gammaray-client ${gammaray_client_srcs})

target_link_libraries(gammaray-client ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} gammaray_network)

