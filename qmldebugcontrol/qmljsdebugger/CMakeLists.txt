project(qmljsdebugger)

#find_package(Qt4 REQUIRED)

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

add_subdirectory(editor)
add_subdirectory(protocol)

set(qmljsdebugger_SRCS
    jsdebuggeragent.cpp
    qdeclarativeviewinspector_p.cpp
    qdeclarativeinspectorservice.cpp
    qdeclarativeviewobserver.cpp
)

qt4_automoc(${qmljsdebugger_SRCS})

add_definitions(-DBUILD_QMLJSDEBUGGER_STATIC_LIB)

add_library(QmlJSDebugger STATIC ${qmljsdebugger_SRCS})

target_link_libraries(QmlJSDebugger
  ${QT_QTDECLARATIVE_LIBRARIES}
  ${QT_QTSCRIPT_LIBRARIES}
  editor
  protocol
)
