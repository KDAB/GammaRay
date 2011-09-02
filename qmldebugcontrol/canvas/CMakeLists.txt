project(canvas)

set(canvas_SRCS
  qdeclarativecanvas.cpp
  qdeclarativecanvastimer.cpp
  qdeclarativecontext2d.cpp
  qdeclarativetiledcanvas.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}

)

add_library(canvas ${canvas_SRCS})

qt4_automoc(${canvas_SRCS})

target_link_libraries(canvas
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTDECLARATIVE_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
)
