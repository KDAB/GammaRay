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

  ${CMAKE_SOURCE_DIR}/3rdparty/other/modelutils.cpp

  sidepane.cpp
  palettemodel.cpp
  aboutpluginsdialog.cpp
  mainwindow.cpp
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
  mainwindow.ui

  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  propertyeditor/palettedialog.ui
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

qt4_automoc(${gammaray_ui_srcs})

add_library(gammaray_ui STATIC ${gammaray_ui_srcs})

target_link_libraries(gammaray_ui ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES})
