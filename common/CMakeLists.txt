# generate the class icon index map
file(READ "../ui/resources/classiconindex.map" _map_content)
string(REPLACE "\n" ";" _map_content ${_map_content})
set(CLASS_ICON_INDEX_CODE "")
foreach(_icon_data IN LISTS _map_content)
  if(_icon_data)
    string(REPLACE " " ";" _icon_data ${_icon_data})
    list(GET _icon_data 0 _icon_name)
    set(CLASS_ICON_INDEX_CODE "${CLASS_ICON_INDEX_CODE}\n    ADD_ICON(\"${_icon_name}\")")
  endif()
endforeach()
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsindex_data.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/classesiconsindex_data.cpp
)

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
  objectidfilterproxymodel.cpp
  paintanalyzerinterface.cpp
  selflocator.cpp
  sourcelocation.cpp
  translator.cpp

  enumdefinition.cpp
  enumrepository.cpp
  enumvalue.cpp
  classesiconsrepository.cpp

  remoteviewinterface.cpp
  remoteviewframe.cpp
  transferimage.cpp

  commonutils.cpp
)

add_library(gammaray_common ${GAMMARAY_LIBRARY_TYPE} ${gammaray_common_srcs})
generate_export_header(gammaray_common)
set_target_properties(gammaray_common PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_common-${GAMMARAY_PROBE_ABI}
)
target_compile_features(gammaray_common PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

gammaray_target_relocatable_interfaces(gammaray_common_ipaths)
target_include_directories(gammaray_common PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_common_ipaths}>)
target_link_libraries(gammaray_common LINK_PUBLIC Qt::Core LINK_PRIVATE Qt::Network Qt::Gui)
target_link_libraries(gammaray_common LINK_PRIVATE gammaray_kitemmodels ${CMAKE_DL_LIBS})
gammaray_set_rpath(gammaray_common ${LIB_INSTALL_DIR})

set(gammaray_common_internal_srcs
  plugininfo.cpp
  pluginmanager.cpp
  proxyfactorybase.cpp
  propertycontrollerinterface.cpp
  probecontrollerinterface.cpp
  toolmanagerinterface.cpp
  networkselectionmodel.cpp
  streamoperators.cpp

  tools/objectinspector/propertiesextensioninterface.cpp
  tools/objectinspector/methodsextensioninterface.cpp
  tools/objectinspector/connectionsextensioninterface.cpp
  tools/messagehandler/messagehandlerinterface.cpp
  tools/metatypebrowser/metatypebrowserinterface.cpp
  tools/problemreporter/problemreporterinterface.cpp
  tools/resourcebrowser/resourcebrowserinterface.cpp
)

add_library(gammaray_common_internal STATIC ${gammaray_common_internal_srcs})
set_target_properties(gammaray_common_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_common_internal PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

target_link_libraries(gammaray_common_internal gammaray_common)
target_link_libraries(gammaray_common_internal Qt::Core Qt::Network)

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_common EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

  gammaray_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_common_export.h
    endpoint.h
    enumdefinition.h
    enumrepository.h
    enumvalue.h
    metatypedeclarations.h
    modelroles.h
    objectbroker.h
    objectid.h
    objectmodel.h
    objectidfilterproxymodel.h
    paths.h
    probecontrollerinterface.h
    propertycontrollerinterface.h
    protocol.h
    sourcelocation.h
    translator.h
    commonutils.h
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
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_common>/$<TARGET_PDB_FILE_NAME:gammaray_common>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
