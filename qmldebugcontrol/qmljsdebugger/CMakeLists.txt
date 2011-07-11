
cmake_minimum_required(VERSION 2.8)

project(qmljsdebugger)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${QT_INCLUDES}
)

remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

set(qmljsdebugger_SRCS
    jsdebuggeragent.cpp
    qdeclarativeviewinspector.cpp
    qdeclarativeinspectorservice.cpp
    editor/abstractliveedittool.cpp
    editor/liveselectiontool.cpp
    editor/livelayeritem.cpp
    editor/livesingleselectionmanipulator.cpp
    editor/liverubberbandselectionmanipulator.cpp
    editor/liveselectionrectangle.cpp
    editor/liveselectionindicator.cpp
    editor/boundingrecthighlighter.cpp
    editor/subcomponentmasklayeritem.cpp
    editor/zoomtool.cpp
    editor/colorpickertool.cpp
    editor/qmltoolbar.cpp
    editor/toolbarcolorbox.cpp
)

qt4_automoc(${qmljsdebugger_SRCS})

add_definitions(-DBUILD_QMLJSDEBUGGER_STATIC_LIB)

add_library(QmlJSDebugger STATIC ${qmljsdebugger_SRCS})

target_link_libraries(QmlJSDebugger ${QT_QTDECLARATIVE_LIBRARIES} ${QT_QTSCRIPT_LIBRARIES})
