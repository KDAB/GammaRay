set(gammaray_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/qt/resourcemodel.cpp

  aggregatedpropertymodel.cpp
  metaobject.cpp
  metaobjecttreemodel.cpp
  metaobjectrepository.cpp
  metaproperty.cpp
  probe.cpp
  probeguard.cpp
  probesettings.cpp
  probecontroller.cpp
#  proxydetacher.cpp
  objectlistmodel.cpp
  objectclassinfomodel.cpp
  objectmethodmodel.cpp
  objectenummodel.cpp
  objecttreemodel.cpp
  connectionmodel.cpp
  connectionfilterproxymodel.cpp
  methodargumentmodel.cpp
  multisignalmapper.cpp
  signalspycallbackset.cpp
  toolmodel.cpp
  toolpluginmodel.cpp
  toolpluginerrormodel.cpp
  propertycontroller.cpp
  propertycontrollerextension.cpp
  proxytoolfactory.cpp
  util.cpp
  varianthandler.cpp

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

  tools/modelinspector/modeltester.cpp
  tools/modelinspector/modelmodel.cpp
  tools/modelinspector/modelcellmodel.cpp
  tools/metatypebrowser/metatypesmodel.cpp
  tools/localeinspector/localemodel.cpp
  tools/localeinspector/localedataaccessor.cpp
  tools/localeinspector/localeaccessormodel.cpp
  tools/textdocumentinspector/textdocumentmodel.cpp
  tools/textdocumentinspector/textdocumentformatmodel.cpp
  tools/messagehandler/messagehandler.cpp
  tools/messagehandler/messagehandlerinterface.cpp
  tools/messagehandler/messagemodel.cpp
  tools/connectioninspector/connectioninspector.cpp
  tools/localeinspector/localeinspector.cpp
  tools/metaobjectbrowser/metaobjectbrowser.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/modelinspector/modelinspector.cpp
  tools/modelinspector/safetyfilterproxymodel.cpp
  tools/objectinspector/objectinspector.cpp
  tools/objectinspector/propertiesextension.cpp
  tools/objectinspector/methodsextension.cpp
  tools/objectinspector/connectionsextension.cpp
  tools/objectinspector/abstractconnectionsmodel.cpp
  tools/objectinspector/inboundconnectionsmodel.cpp
  tools/objectinspector/outboundconnectionsmodel.cpp
  tools/objectinspector/enumsextension.cpp
  tools/objectinspector/classinfoextension.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp

  remote/server.cpp
  remote/remotemodelserver.cpp
  remote/selectionmodelserver.cpp
  remote/serverdevice.cpp
  remote/tcpserverdevice.cpp
  remote/localserverdevice.cpp
  remote/serverproxymodel.cpp
)

if(Qt5Core_FOUND)
  set(gammaray_srcs
    ${gammaray_srcs}
    tools/mimetypes/mimetypes.cpp
    tools/mimetypes/mimetypesmodel.cpp
    tools/standardpaths/standardpaths.cpp
    tools/standardpaths/standardpathsmodel.cpp
  )
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

qt4_wrap_cpp(gammaray_srcs ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.h)
qt4_add_resources(gammaray_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_definitions(-DMAKE_GAMMARAY_CORE_LIB)

# core lib
add_library(gammaray_core SHARED
  ${gammaray_srcs}
)

target_link_libraries(gammaray_core
  LINK_PUBLIC gammaray_common
  LINK_PRIVATE gammaray_common_internal
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

install(TARGETS gammaray_core EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

gammaray_install_headers(
  gammaray_core_export.h
  metaobject.h
  metaobjectrepository.h
  metaproperty.h
  objectmodelbase.h
  objecttypefilterproxymodel.h
  probeinterface.h
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
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
