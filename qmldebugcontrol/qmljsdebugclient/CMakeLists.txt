project(qmldebugclient)

set(qmldebugclient_SRCS
    qdeclarativedebug.cpp
    qdeclarativedebugclient.cpp
    qpacketprotocol.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}

)

# qt4_generate_moc(${_testname}.cpp ${moc_output_file})
# qt4_wrap_cpp(${qmldebugclient_SRCS})
qt4_automoc(${qmldebugclient_SRCS})
# qt4_generate_moc(qpacketprotocol.cpp )

add_library(qmldebugclient ${qmldebugclient_SRCS})

target_link_libraries(qmldebugclient
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTNETWORK_LIBRARIES}
)
