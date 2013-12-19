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
  objectbroker.cpp
  protocol.cpp
  message.cpp
  endpoint.cpp
)

add_library(gammaray_common SHARED ${gammaray_common_srcs})
set_target_properties(gammaray_common PROPERTIES
  DEFINE_SYMBOL MAKE_GAMMARAY_COMMON_LIB
  SOVERSION ${GAMMARAY_SOVERSION}
  VERSION ${GAMMARAY_SOVERSION}
  OUTPUT_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common Qt5::Core Qt5::Network)
else()
  target_link_libraries(gammaray_common
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
  )
endif()

install(TARGETS gammaray_common ${INSTALL_TARGETS_DEFAULT_ARGS})

set(gammaray_common_internal_srcs
  pluginmanager.cpp
  proxyfactorybase.cpp
  propertycontrollerinterface.cpp
  probecontrollerinterface.cpp
  modelinspectorinterface.cpp
  resourcebrowserinterface.cpp
  networkselectionmodel.cpp
  streamoperators.cpp
)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

add_library(gammaray_common_internal STATIC ${gammaray_common_internal_srcs})

target_link_libraries(gammaray_common_internal gammaray_common)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common_internal Qt5::Gui Qt5::Network)
else()
  target_link_libraries(gammaray_common_internal
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
  )
endif()

gammaray_install_headers(
  gammaray_common_export.h
  endpoint.h
  metatypedeclarations.h
  modelroles.h
  objectbroker.h
  objectmodel.h
  propertycontrollerinterface.h
  protocol.h
)
