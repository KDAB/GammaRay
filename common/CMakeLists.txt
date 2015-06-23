#
# Shared non-UI code between probe and client
#
set(gammaray_common_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/klinkitemselectionmodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kmodelindexproxymapper.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/lz4/lz4.c

  methodargument.cpp
  objectbroker.cpp
  protocol.cpp
  message.cpp
  endpoint.cpp
  paths.cpp
  propertysyncer.cpp
)

add_library(gammaray_common SHARED ${gammaray_common_srcs})
set_target_properties(gammaray_common PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  DEFINE_SYMBOL MAKE_GAMMARAY_COMMON_LIB
  OUTPUT_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common LINK_PUBLIC Qt5::Core LINK_PRIVATE Qt5::Network)
else()
  target_link_libraries(gammaray_common
    LINK_PUBLIC ${QT_QTCORE_LIBRARIES}
    LINK_PRIVATE ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
  )
endif()

install(TARGETS gammaray_common EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

set(gammaray_common_internal_srcs
  plugininfo.cpp
  pluginmanager.cpp
  proxyfactorybase.cpp
  propertycontrollerinterface.cpp
  probecontrollerinterface.cpp
  modelinspectorinterface.cpp
  resourcebrowserinterface.cpp
  networkselectionmodel.cpp
  streamoperators.cpp
  probeabi.cpp
  probeabidetector.cpp

  tools/objectinspector/propertiesextensioninterface.cpp
  tools/objectinspector/methodsextensioninterface.cpp
  tools/objectinspector/connectionsextensioninterface.cpp
)

if(WIN32)
  list(APPEND gammaray_common_internal_srcs probeabidetector_win.cpp)
elseif(APPLE)
  list(APPEND gammaray_common_internal_srcs probeabidetector_mac.cpp)
elseif(UNIX AND NOT QNXNTO)
  list(APPEND gammaray_common_internal_srcs probeabidetector_elf.cpp)
else()
  list(APPEND gammaray_common_internal_srcs probeabidetector_dummy.cpp)
endif()

add_library(gammaray_common_internal STATIC ${gammaray_common_internal_srcs})
set_target_properties(gammaray_common_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)

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

if(WIN32)
  target_link_libraries(gammaray_common_internal version)
endif()
if(APPLE)
  target_link_libraries(gammaray_common_internal "-framework CoreFoundation")
endif()

if (ANDROID)
    add_definitions(-DENABLE_MESSAGE_COMPRESSSION)
endif()

gammaray_install_headers(
  gammaray_common_export.h
  endpoint.h
  enums.h
  metatypedeclarations.h
  modelroles.h
  objectbroker.h
  objectmodel.h
  paths.h
  propertycontrollerinterface.h
  protocol.h
)

ecm_generate_pri_file(BASE_NAME GammaRayCommon
                      LIB_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
                      DEPS "core gui network"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
