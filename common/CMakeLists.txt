#
# Shared non-UI code between probe and client
#
set(gammaray_common_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/lz4/lz4.c

  methodargument.cpp
  objectbroker.cpp
  protocol.cpp
  message.cpp
  endpoint.cpp
  paths.cpp
  propertysyncer.cpp
  modelevent.cpp
  modelutils.cpp
  paintanalyzerinterface.cpp
  selflocator.cpp
  sourcelocation.cpp
  translator.cpp

  remoteviewinterface.cpp
  remoteviewframe.cpp
  transferimage.cpp
)

add_library(gammaray_common SHARED ${gammaray_common_srcs})
generate_export_header(gammaray_common)
set_target_properties(gammaray_common PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
)

gammaray_target_relocatable_interfaces(gammaray_common_ipaths)
target_include_directories(gammaray_common PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_common_ipaths}>)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common LINK_PUBLIC Qt5::Core LINK_PRIVATE Qt5::Network Qt5::Gui gammaray_kitemmodels)
else()
  target_link_libraries(gammaray_common
    LINK_PUBLIC ${QT_QTCORE_LIBRARIES}
    LINK_PRIVATE ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} gammaray_kitemmodels
  )
endif()
if(NOT WIN32 AND NOT QNXNTO AND NOT ANDROID)
  target_link_libraries(gammaray_common LINK_PRIVATE dl)
endif()


set(gammaray_common_internal_srcs
  plugininfo.cpp
  pluginmanager.cpp
  proxyfactorybase.cpp
  propertycontrollerinterface.cpp
  probecontrollerinterface.cpp
  modelinspectorinterface.cpp
  networkselectionmodel.cpp
  streamoperators.cpp

  tools/objectinspector/propertiesextensioninterface.cpp
  tools/objectinspector/methodsextensioninterface.cpp
  tools/objectinspector/connectionsextensioninterface.cpp
  tools/messagehandler/messagehandlerinterface.cpp
  tools/resourcebrowser/resourcebrowserinterface.cpp
)

add_library(gammaray_common_internal STATIC ${gammaray_common_internal_srcs})
set_target_properties(gammaray_common_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(gammaray_common_internal gammaray_common)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common_internal Qt5::Core Qt5::Network)
else()
  target_link_libraries(gammaray_common_internal
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
  )
endif()

if (ANDROID)
    add_definitions(-DENABLE_MESSAGE_COMPRESSSION)
endif()

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_common EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

  gammaray_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_common_export.h
    endpoint.h
    metatypedeclarations.h
    modelroles.h
    objectbroker.h
    objectmodel.h
    paths.h
    probecontrollerinterface.h
    propertycontrollerinterface.h
    protocol.h
    sourcelocation.h
    translator.h
  )

  ecm_generate_pri_file(BASE_NAME GammaRayCommon
                        LIB_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
                        DEPS "core gui network"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
  )

  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
else()
  install(TARGETS gammaray_common ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
if(MSVC)
  install(FILES "${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray_common-${GAMMARAY_PROBE_ABI}.pdb" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
