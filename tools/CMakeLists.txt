add_subdirectory(objectvisualizer)
add_subdirectory(scriptenginedebugger)
add_subdirectory(statemachineviewer)

if (NOT Qt5Core_FOUND)
  add_subdirectory(webinspector)
endif()
add_subdirectory(widgetinspector)
