#
# Shared non-UI code between probe and client
#

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty/
)

set(gammaray_common_srcs
  methodargument.cpp
  pluginmanager.cpp
  proxytoolfactory.cpp
  
  network/protocol.cpp
  network/message.cpp
  network/endpoint.cpp
  network/objectbroker.cpp
  network/networkselectionmodel.cpp
  network/networkobject.cpp
  network/streamoperators.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

qt4_automoc(${gammaray_common_srcs})

add_library(gammaray_common STATIC ${gammaray_common_srcs})

target_link_libraries(gammaray_common ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
