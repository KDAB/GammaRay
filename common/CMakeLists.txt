if(NOT CMAKE_CROSSCOMPILING)
    # classes icons tool
    add_executable(classesiconsextractor classesiconsextractor.cpp)
    if(Qt5Core_FOUND)
      target_link_libraries(classesiconsextractor LINK_PUBLIC Qt5::Core Qt5::Xml)
      query_qmake(QT_HOST_BINS QT_HOST_BINS)
    else()
      set(QT_HOST_BINS ${QT_BINARY_DIR})
      target_link_libraries(classesiconsextractor LINK_PUBLIC ${QT_QTCORE_LIBRARIES} ${QT_QTXML_LIBRARIES})
    endif()

    if (WIN32)
      set(_PATH_SEP ";")
    else()
      set(_PATH_SEP ":")
    endif()

    # we prepend an extra _PATH_SEP to work around an escaping issue with Windows path separators and quotes
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsindex_data.cpp
        COMMAND
            ${CMAKE_COMMAND} -E env "PATH=${QT_HOST_BINS}${_PATH_SEP}$ENV{PATH}${_PATH_SEP}"
            $<TARGET_FILE:classesiconsextractor>
                -qrc ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/ui.qrc
                -out ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsindex_data.cpp
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsextractor.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/ui.qrc
    )
    add_custom_target(classesiconsindex_data DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/classesiconsindex_data.cpp)
endif()

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
if(NOT CMAKE_CROSSCOMPILING)
    add_dependencies(gammaray_common classesiconsindex_data)
endif()

gammaray_target_relocatable_interfaces(gammaray_common_ipaths)
target_include_directories(gammaray_common PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_common_ipaths}>)
if(Qt5Core_FOUND)
  target_link_libraries(gammaray_common LINK_PUBLIC Qt5::Core LINK_PRIVATE Qt5::Network Qt5::Gui)
else()
  target_link_libraries(gammaray_common
    LINK_PUBLIC ${QT_QTCORE_LIBRARIES}
    LINK_PRIVATE ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
  )
endif()
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

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_common EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

  gammaray_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_common_export.h
    endpoint.h
    enumdefinition.h
    enumrepository.h
    enumvalue.h
    classesiconsrepository.h
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
