#
# Shared code between in-process and out-of-process UI
#
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_SOURCE_DIR}/common
)

set(gammaray_ui_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/krecursivefilterproxymodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kfilterproxysearchline.cpp

  palettemodel.cpp
  methodinvocationdialog.cpp
  propertywidget.cpp
  deferredresizemodesetter.cpp

  propertyeditor/propertycoloreditor.cpp
  propertyeditor/propertydoublepaireditor.cpp
  propertyeditor/propertyeditorfactory.cpp
  propertyeditor/propertyextendededitor.cpp
  propertyeditor/propertyfonteditor.cpp
  propertyeditor/propertyintpaireditor.cpp
  propertyeditor/propertypaletteeditor.cpp
  propertyeditor/palettedialog.cpp
)

qt4_wrap_ui(gammaray_ui_srcs
  methodinvocationdialog.ui
  propertywidget.ui

  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  propertyeditor/palettedialog.ui
)

qt4_automoc(${gammaray_ui_srcs})

add_definitions(-DMAKE_GAMMARAY_UI_LIB)

add_library(gammaray_ui SHARED ${gammaray_ui_srcs})

target_link_libraries(gammaray_ui ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common_internal)

install(TARGETS gammaray_ui ${INSTALL_TARGETS_DEFAULT_ARGS})


set(gammaray_ui_internal_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/other/modelutils.cpp

  sidepane.cpp
  aboutpluginsdialog.cpp
  mainwindow.cpp

  tools/codecbrowser/codecbrowserwidget.cpp
  tools/connectioninspector/connectioninspectorwidget.cpp
  tools/localeinspector/localeinspectorwidget.cpp
  tools/messagehandler/messagehandlerwidget.cpp
  tools/metaobjectbrowser/metaobjectbrowserwidget.cpp
  tools/metatypebrowser/metatypebrowserwidget.cpp
  tools/mimetypes/mimetypeswidget.cpp
  tools/modelinspector/modelinspectorwidget.cpp
  tools/objectinspector/objectinspectorwidget.cpp
  tools/standardpaths/standardpathswidget.cpp
)

qt4_wrap_ui(gammaray_ui_internal_srcs
  mainwindow.ui

  tools/codecbrowser/codecbrowserwidget.ui
  tools/connectioninspector/connectioninspectorwidget.ui
  tools/localeinspector/localeinspectorwidget.ui
  tools/messagehandler/messagehandlerwidget.ui
  tools/metatypebrowser/metatypebrowserwidget.ui
  tools/mimetypes/mimetypeswidget.ui
  tools/modelinspector/modelinspectorwidget.ui
  tools/objectinspector/objectinspectorwidget.ui
  tools/standardpaths/standardpathswidget.ui
)

qt4_automoc(${gammaray_ui_internal_srcs})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_library(gammaray_ui_internal STATIC ${gammaray_ui_internal_srcs})

target_link_libraries(gammaray_ui ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common_internal gammaray_ui)




