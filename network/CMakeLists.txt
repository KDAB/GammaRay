include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(gammaray_network_srcs
  protocol.cpp
  message.cpp
  endpoint.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

qt4_automoc(${gammaray_network_srcs})

add_library(gammaray_network STATIC ${gammaray_network_srcs})

target_link_libraries(gammaray_network ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
