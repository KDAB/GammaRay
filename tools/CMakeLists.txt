add_subdirectory(objectvisualizer)
add_subdirectory(scriptenginedebugger)
add_subdirectory(statemachineviewer)

# Disabled in Qt5 mode, unless it was actually found.
if(NOT Qt5Core_FOUND OR Qt5Webkit_FOUND)
  add_subdirectory(webinspector)
endif()

add_subdirectory(widgetinspector)
