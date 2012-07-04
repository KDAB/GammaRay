include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}

  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/krecursivefilterproxymodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kfilterproxysearchline.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/qt/resourcemodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/other/modelutils.cpp

  util.cpp
  aboutpluginsdialog.cpp
  mainwindow.cpp
  metaobject.cpp
  metaobjecttreemodel.cpp
  metaobjectrepository.cpp
  metaproperty.cpp
  metapropertymodel.cpp
  pluginmanager.cpp
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
  methodinvocationdialog.cpp
  methodargumentmodel.cpp
  multisignalmapper.cpp
  toolmodel.cpp
  proxydetacher.cpp
  proxytoolfactory.cpp
  sidepane.cpp
  palettemodel.cpp
  propertywidget.cpp

  propertyeditor/propertycoloreditor.cpp
  propertyeditor/propertydoublepaireditor.cpp
  propertyeditor/propertyeditorfactory.cpp
  propertyeditor/propertyextendededitor.cpp
  propertyeditor/propertyfonteditor.cpp
  propertyeditor/propertyintpaireditor.cpp
  propertyeditor/propertypaletteeditor.cpp
  propertyeditor/palettedialog.cpp

  tools/modelinspector/modeltester.cpp
  tools/modelinspector/modelmodel.cpp
  tools/modelinspector/modelcellmodel.cpp
  tools/sceneinspector/graphicsview.cpp
  tools/sceneinspector/graphicssceneview.cpp
  tools/sceneinspector/scenemodel.cpp
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
  tools/modelinspector/modelinspectorwidget.cpp
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/sceneinspector/sceneinspector.cpp
  tools/selectionmodelinspector/selectionmodelinspector.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/widgetinspector/widgetinspector.cpp
  tools/widgetinspector/overlaywidget.cpp
  tools/widgetinspector/widgettreemodel.cpp
  tools/widgetinspector/widgetpreviewwidget.cpp
  tools/styleinspector/styleinspector.cpp
  tools/styleinspector/pixelmetricmodel.cpp
  tools/styleinspector/standardiconmodel.cpp
  tools/styleinspector/primitivemodel.cpp
  tools/styleinspector/controlmodel.cpp
  tools/styleinspector/styleoption.cpp
  tools/styleinspector/abstractstyleelementmodel.cpp
  tools/styleinspector/abstractstyleelementstatetable.cpp
  tools/styleinspector/styleelementstatetablepage.cpp
  tools/styleinspector/complexcontrolmodel.cpp
  tools/styleinspector/dynamicproxystyle.cpp
)

if(Qt5Core_FOUND)
  set(gammaray_srcs
    ${gammaray_srcs}
    tools/mimetypes/mimetypes.cpp
    tools/standardpaths/standardpaths.cpp
    tools/standardpaths/standardpathsmodel.cpp
  )

  qt4_wrap_ui(gammaray_srcs
    tools/mimetypes/mimetypes.ui
    tools/standardpaths/standardpaths.ui
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
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
  paintbufferviewer.ui

  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  propertyeditor/palettedialog.ui

  tools/sceneinspector/graphicssceneview.ui
  tools/codecbrowser/codecbrowser.ui
  tools/connectioninspector/connectioninspector.ui
  tools/fontbrowser/fontbrowser.ui
  tools/localeinspector/localeinspector.ui
  tools/messagehandler/messagehandler.ui
  tools/metatypebrowser/metatypebrowser.ui
  tools/modelinspector/modelinspectorwidget.ui
  tools/objectinspector/objectinspector.ui
  tools/resourcebrowser/resourcebrowser.ui
  tools/sceneinspector/sceneinspector.ui
  tools/selectionmodelinspector/selectionmodelinspector.ui
  tools/styleinspector/styleinspector.ui
  tools/textdocumentinspector/textdocumentinspector.ui
  tools/widgetinspector/widgetinspector.ui
  tools/styleinspector/styleelementstatetablepage.ui
)

qt4_add_resources(gammaray_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_definitions(-DMAKE_GAMMARAY_LIB)

# core lib
add_library(gammaray_core SHARED
  ${gammaray_srcs}
)

if(Qt5Core_FOUND)
  target_link_libraries(gammaray_core Qt5::Widgets)
else()
  target_link_libraries(gammaray_core
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
  )
endif()

if(NOT WIN32)
  target_link_libraries(gammaray_core dl)
endif()
install(TARGETS gammaray_core ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(hooking)
add_subdirectory(tools)
