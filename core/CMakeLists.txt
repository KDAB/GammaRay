include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/qt/resourcemodel.cpp

  metaobject.cpp
  metaobjecttreemodel.cpp
  metaobjectrepository.cpp
  metaproperty.cpp
  metapropertymodel.cpp
  probe.cpp
  probesettings.cpp
  probecontroller.cpp
#  proxydetacher.cpp
  objectlistmodel.cpp
  objectpropertymodel.cpp
  objectdynamicpropertymodel.cpp
  objectstaticpropertymodel.cpp
  objectclassinfomodel.cpp
  objectmethodmodel.cpp
  objectenummodel.cpp
  objecttreemodel.cpp
  connectionmodel.cpp
  connectionfilterproxymodel.cpp
  methodargumentmodel.cpp
  multisignalmapper.cpp
  toolmodel.cpp
  toolpluginmodel.cpp
  toolpluginerrormodel.cpp
  propertycontroller.cpp
  proxytoolfactory.cpp
  util.cpp
  varianthandler.cpp

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
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp

  remote/server.cpp
  remote/remotemodelserver.cpp
  remote/selectionmodelserver.cpp
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
    tools/messagehandler/StackWalker.cpp)
endif()

qt4_wrap_cpp(gammaray_srcs tools/modelinspector/modeltest.h)
qt4_add_resources(gammaray_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_definitions(-DMAKE_GAMMARAY_CORE_LIB)

# core lib
add_library(gammaray_core SHARED
  ${gammaray_srcs}
)

target_link_libraries(gammaray_core
  gammaray_common
  gammaray_common_internal
)
set_target_properties(gammaray_core PROPERTIES
  SOVERSION ${GAMMARAY_SOVERSION}
  VERSION ${GAMMARAY_SOVERSION}
  OUTPUT_NAME gammaray_core-${GAMMARAY_PROBE_ABI}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_core Qt5::Gui Qt5::Test)
else()
  target_link_libraries(gammaray_core
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
  )
endif()

if(NOT WIN32 AND NOT QNXNTO)
  target_link_libraries(gammaray_core dl)
endif()
install(TARGETS gammaray_core ${INSTALL_TARGETS_DEFAULT_ARGS})

gammaray_install_headers(
  metaobject.h
  metaobjectrepository.h
  objectmodelbase.h
  objecttypefilterproxymodel.h
  probeinterface.h
  propertycontroller.h
  singlecolumnobjectproxymodel.h
  toolfactory.h
  util.h
  varianthandler.h
)
