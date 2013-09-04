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
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/klinkitemselectionmodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kmodelindexproxymapper.cpp

  methodargument.cpp
  util.cpp

  network/objectbroker.cpp
  network/protocol.cpp
  network/message.cpp
  network/endpoint.cpp
)

qt4_automoc(${gammaray_common_srcs})

add_library(gammaray_common SHARED ${gammaray_common_srcs})
set_target_properties(gammaray_common PROPERTIES DEFINE_SYMBOL MAKE_GAMMARAY_COMMON_LIB)

target_link_libraries(gammaray_common ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
install(TARGETS gammaray_common ${INSTALL_TARGETS_DEFAULT_ARGS})

set(gammaray_common_internal_srcs
  pluginmanager.cpp
  proxytoolfactory.cpp
  propertycontrollerinterface.cpp
  probecontrollerinterface.cpp

  network/networkselectionmodel.cpp
  network/streamoperators.cpp
)

qt4_automoc(${gammaray_common_internal_srcs})

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()
add_library(gammaray_common_internal STATIC ${gammaray_common_internal_srcs})

target_link_libraries(gammaray_common_internal gammaray_common ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})


