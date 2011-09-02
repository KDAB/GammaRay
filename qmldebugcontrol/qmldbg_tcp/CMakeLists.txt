project(qlmdbg_tcp)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(qmldbg_tcp_SRCS
  qtcpserverconnection.cpp
)

qt4_automoc(${qmldbg_tcp_SRCS})

add_library(qmldbg_tcp MODULE ${qmldbg_tcp_SRCS})

target_link_libraries(qmldbg_tcp
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
)
