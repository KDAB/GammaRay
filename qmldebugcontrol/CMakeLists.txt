project(qmldebugcontrol)

include(${QT_USE_FILE})

remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(qmljsdebugclient)
add_subdirectory(canvas)
add_subdirectory(qmljsdebugger)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/qmljsdebugclient
)

set(qmldebugcontrol_SRCS
    qmlprofilertool.cpp
    qmlprofilereventview.cpp
    tracewindow.cpp
    timelineview.cpp
    tracewindow.cpp
    qmlprofilertraceclient.cpp
    main.cpp
)

qt4_add_resources(_testresource_rcc_src qml/qml.qrc OPTIONS -root "/")

qt4_automoc(${qmldebugcontrol_SRCS})

add_executable(qmldebugcontrol ${qmldebugcontrol_SRCS} ${_testresource_rcc_src})

target_link_libraries(qmldebugcontrol
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTSCRIPT_LIBRARIES}
  ${QT_QTNETWORK_LIBRARIES}
  qmldebugclient
  canvas
)

