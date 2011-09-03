project(editor)

set(editor_SRCS
  abstractliveedittool.cpp
  liveselectiontool.cpp
  livelayeritem.cpp
  livesingleselectionmanipulator.cpp
  liverubberbandselectionmanipulator.cpp
  liveselectionrectangle.cpp
  liveselectionindicator.cpp
  boundingrecthighlighter.cpp
  subcomponentmasklayeritem.cpp
  zoomtool.cpp
  colorpickertool.cpp
  qmltoolbar.cpp
  toolbarcolorbox.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

qt4_automoc(
  abstractliveedittool.cpp
  liveselectiontool.cpp
  livelayeritem.cpp
  boundingrecthighlighter.cpp
  zoomtool.cpp
  colorpickertool.cpp
  qmltoolbar.cpp
  toolbarcolorbox.cpp
)

qt4_add_resources(_editor_rcc_src editor.qrc OPTIONS -root "/")

add_library(editor ${editor_SRCS} ${_editor_rcc_src})

target_link_libraries(editor
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
)
