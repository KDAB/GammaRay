set(gammaray_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/qt/resourcemodel.cpp

  aggregatedpropertymodel.cpp
  metaobject.cpp
  metaobjectregistry.cpp
  metaobjectrepository.cpp
  metaproperty.cpp
  probe.cpp
  probeguard.cpp
  probesettings.cpp
  probecontroller.cpp
  objectlistmodel.cpp
  objectclassinfomodel.cpp
  objectmethodmodel.cpp
  objectenummodel.cpp
  objecttreemodel.cpp
  objecttypefilterproxymodel.cpp
  methodargumentmodel.cpp
  multisignalmapper.cpp
  signalspycallbackset.cpp
  singlecolumnobjectproxymodel.cpp
  toolfactory.cpp
  toolmanager.cpp
  toolpluginmodel.cpp
  toolpluginerrormodel.cpp
  propertycontroller.cpp
  propertycontrollerextension.cpp
  proxytoolfactory.cpp
  util.cpp
  varianthandler.cpp
  objectdataprovider.cpp
  attributemodel.cpp
  qmetaobjectvalidator.cpp
  enumrepositoryserver.cpp
  enumutil.cpp
  execution.cpp
  classesiconsrepositoryserver.cpp

  propertyadaptor.cpp
  propertyaggregator.cpp
  propertydata.cpp
  dynamicpropertyadaptor.cpp
  qmetapropertyadaptor.cpp
  metapropertyadaptor.cpp
  associativepropertyadaptor.cpp
  sequentialpropertyadaptor.cpp
  objectinstance.cpp
  propertyadaptorfactory.cpp

  paintbuffermodel.cpp
  paintanalyzer.cpp

  remoteviewserver.cpp

  tools/bindinginspector/abstractbindingprovider.cpp
  tools/bindinginspector/bindingextension.cpp
  tools/bindinginspector/bindingmodel.cpp
  tools/bindinginspector/bindingnode.cpp
  tools/metatypebrowser/metatypesmodel.cpp
  tools/messagehandler/messagehandler.cpp
  tools/messagehandler/messagemodel.cpp
  tools/metaobjectbrowser/metaobjectbrowser.cpp
  tools/metaobjectbrowser/metaobjecttreemodel.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/objectinspector/objectinspector.cpp
  tools/objectinspector/propertiesextension.cpp
  tools/objectinspector/methodsextension.cpp
  tools/objectinspector/connectionsextension.cpp
  tools/objectinspector/abstractconnectionsmodel.cpp
  tools/objectinspector/inboundconnectionsmodel.cpp
  tools/objectinspector/outboundconnectionsmodel.cpp
  tools/objectinspector/enumsextension.cpp
  tools/objectinspector/classinfoextension.cpp
  tools/objectinspector/applicationattributeextension.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp

  remote/server.cpp
  remote/remotemodelserver.cpp
  remote/selectionmodelserver.cpp
  remote/serverdevice.cpp
  remote/tcpserverdevice.cpp
  remote/localserverdevice.cpp
  remote/serverproxymodel.cpp

  ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc
)

if(Qt5Core_FOUND)
  set(gammaray_srcs
    ${gammaray_srcs}
    tools/messagehandler/loggingcategorymodel.cpp
    tools/standardpaths/standardpaths.cpp
    tools/standardpaths/standardpathsmodel.cpp
  )
endif()

if(NOT Qt5Gui_VERSION VERSION_LESS 5.5.0) # QPaintBuffer was removed in 5.5
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/)
  list(APPEND gammaray_srcs ${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/private/qpaintbuffer.cpp)
endif()

if(NOT WIN32)
  set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/backtrace_unix.cpp)
elseif(MINGW)
  set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/backtrace_dummy.cpp)
else()
  set(gammaray_srcs ${gammaray_srcs}
    tools/messagehandler/backtrace_win.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/StackWalker/StackWalker.cpp)
endif()

# core lib
add_library(gammaray_core ${GAMMARAY_LIBRARY_TYPE}
  ${gammaray_srcs}
)
generate_export_header(gammaray_core)

gammaray_target_relocatable_interfaces(gammaray_core_ipaths)
target_include_directories(gammaray_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_core_ipaths}>)
target_include_directories(gammaray_core SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/backward-cpp
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries(gammaray_core
  LINK_PUBLIC gammaray_common
  LINK_PRIVATE gammaray_common_internal gammaray_kitemmodels ${DL_LIBRARY}
)
set_target_properties(gammaray_core PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_core-${GAMMARAY_PROBE_ABI}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_core LINK_PUBLIC Qt5::Core LINK_PRIVATE Qt5::Gui)
else()
  target_link_libraries(gammaray_core
    LINK_PUBLIC ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} # GUI is needed for QItemSelectionModel
    LINK_PRIVATE ${QT_QTNETWORK_LIBRARIES}
  )
endif()

add_backward(gammaray_core)

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_core EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

  gammaray_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_core_export.h
    enumrepositoryserver.h
    enumutil.h
    classesiconsrepositoryserver.h
    metaobject.h
    metaobjectrepository.h
    metaproperty.h
    objectmodelbase.h
    objectdataprovider.h
    objecttypefilterproxymodel.h
    probe.h
    probeinterface.h
    probecontroller.h
    propertycontroller.h
    propertycontrollerextension.h
    signalspycallbackset.h
    singlecolumnobjectproxymodel.h
    toolfactory.h
    util.h
    varianthandler.h
  )

  ecm_generate_pri_file(BASE_NAME GammaRayCore
                        LIB_NAME gammaray_core-${GAMMARAY_PROBE_ABI}
                        DEPS "core gui GammaRayCommon"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
  )

  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
  if(GAMMARAY_STATIC_PROBE)
    gammaray_install_headers(staticprobe.h)
  endif()
else()
  install(TARGETS gammaray_core ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_core>/$<TARGET_PDB_FILE_NAME:gammaray_core>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
