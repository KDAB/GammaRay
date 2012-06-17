include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

add_subdirectory(actioninspector)
add_subdirectory(objectvisualizer)
add_subdirectory(scriptenginedebugger)
add_subdirectory(statemachineviewer)
add_subdirectory(timertop)

if(QT_QTWEBKIT_FOUND OR Qt5Webkit_FOUND)
  add_subdirectory(webinspector)
endif()
