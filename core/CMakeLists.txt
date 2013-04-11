include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_SOURCE_DIR}/common
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/ui
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
  proxydetacher.cpp
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
  proxydetacher.cpp
  propertycontroller.cpp

  tools/modelinspector/modeltester.cpp
  tools/modelinspector/modelmodel.cpp
  tools/modelinspector/modelcellmodel.cpp
  tools/metatypebrowser/metatypesmodel.cpp
  tools/fontbrowser/fontmodel.cpp
  tools/localeinspector/localemodel.cpp
  tools/localeinspector/localedataaccessor.cpp
  tools/localeinspector/localeaccessormodel.cpp
  tools/codecbrowser/codecmodel.cpp
  tools/textdocumentinspector/textdocumentmodel.cpp
  tools/textdocumentinspector/textdocumentformatmodel.cpp
  tools/textdocumentinspector/textdocumentcontentview.cpp
  tools/messagehandler/messagehandler.cpp
  tools/messagehandler/messagemodel.cpp
  tools/codecbrowser/codecbrowser.cpp
  tools/connectioninspector/connectioninspector.cpp
  tools/fontbrowser/fontbrowser.cpp
  tools/localeinspector/localeinspector.cpp
  tools/metaobjectbrowser/metaobjectbrowser.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/modelinspector/modelinspector.cpp
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/selectionmodelinspector/selectionmodelinspector.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/widgetinspector/widgetinspector.cpp
  tools/widgetinspector/overlaywidget.cpp
  tools/widgetinspector/widgettreemodel.cpp
  tools/widgetinspector/widgetpreviewwidget.cpp

  remote/server.cpp
  remote/remotemodelserver.cpp
  remote/selectionmodelserver.cpp
  remote/objectserver.cpp
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

if(WIN32 OR APPLE)
  set(gammaray_srcs
    ${gammaray_srcs}
    hooking/abstractfunctionoverwriter.cpp
    hooking/functionoverwriterfactory.cpp
    hooking/winfunctionoverwriter.cpp
    hooking/unixfunctionoverwriter.cpp
  )
endif()

if(HAVE_PRIVATE_QT_HEADERS)
  set(gammaray_srcs
    ${gammaray_srcs}
    paintbuffermodel.cpp
    paintbufferviewer.cpp
    paintbufferreplaywidget.cpp
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

qt4_automoc(${gammaray_srcs})
qt4_wrap_ui(gammaray_srcs
  paintbufferviewer.ui

  tools/fontbrowser/fontbrowser.ui
  tools/resourcebrowser/resourcebrowser.ui
  tools/selectionmodelinspector/selectionmodelinspector.ui
  tools/textdocumentinspector/textdocumentinspector.ui
  tools/widgetinspector/widgetinspector.ui
)

qt4_add_resources(gammaray_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_definitions(-DMAKE_GAMMARAY_CORE_LIB)

# core lib
add_library(gammaray_core SHARED
  ${gammaray_srcs}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_core gammaray_common gammaray_common_internal gammaray_ui Qt5::Widgets)
else()
  target_link_libraries(gammaray_core
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    gammaray_common
    gammaray_common_internal
    gammaray_ui
  )
endif()

if(NOT WIN32)
  target_link_libraries(gammaray_core dl)
endif()
install(TARGETS gammaray_core ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(tools)
