add_subdirectory(objectvisualizer)
add_subdirectory(scriptenginedebugger)
add_subdirectory(statemachineviewer)

if(QT_QTWEBKIT_FOUND OR Qt5Webkit_FOUND)
  add_subdirectory(webinspector)
endif()

# Disabled in Qt5 - QFormBuilder include is not installed
if(NOT Qt5Core_FOUND)
  add_subdirectory(widgetinspector)
endif()
