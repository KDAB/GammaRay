include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

add_subdirectory(codecbrowser)
add_subdirectory(fontbrowser)
add_subdirectory(objectvisualizer)
add_subdirectory(selectionmodelinspector)
add_subdirectory(statemachineviewer)
add_subdirectory(timertop)

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_subdirectory(actioninspector)
  add_subdirectory(widgetinspector)
  add_subdirectory(sceneinspector)
  add_subdirectory(styleinspector)
endif()

if(QT_QTSCRIPTTOOLS_FOUND OR Qt5ScriptTools_FOUND)
  add_subdirectory(scriptenginedebugger)
endif()

if(NOT Qt5Core_FOUND)
  add_subdirectory(kjobtracker)
endif()

if(QT_QTWEBKIT_FOUND OR Qt5WebKitWidgets_FOUND)
  add_subdirectory(webinspector)
endif()

