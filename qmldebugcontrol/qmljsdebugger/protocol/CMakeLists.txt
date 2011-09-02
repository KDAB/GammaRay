project(protocol)

set(protocol_SRCS
  inspectorprotocol.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

qt4_automoc(${protocol_SRCS})

add_library(protocol ${protocol_SRCS})
target_link_libraries(protocol
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
)
