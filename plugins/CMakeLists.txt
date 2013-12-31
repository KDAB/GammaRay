add_subdirectory(codecbrowser)
add_subdirectory(fontbrowser)
add_subdirectory(kjobtracker)
add_subdirectory(objectvisualizer)
add_subdirectory(selectionmodelinspector)
add_subdirectory(statemachineviewer)
add_subdirectory(timertop)
add_subdirectory(webinspector)

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_subdirectory(actioninspector)
  add_subdirectory(widgetinspector)
  add_subdirectory(sceneinspector)
  add_subdirectory(styleinspector)
endif()

if(QT_QTSCRIPTTOOLS_FOUND OR Qt5ScriptTools_FOUND)
  add_subdirectory(scriptenginedebugger)
endif()


