add_subdirectory(codecbrowser)
add_subdirectory(eventmonitor)
add_subdirectory(fontbrowser)
add_subdirectory(kjobtracker)
add_subdirectory(localeinspector)
add_subdirectory(modelinspector)
add_subdirectory(quickinspector)
add_subdirectory(signalmonitor)
add_subdirectory(statemachineviewer)
add_subdirectory(timertop)

add_subdirectory(mimetypes)
add_subdirectory(network)
add_subdirectory(objectvisualizer)
add_subdirectory(qtivi)
add_subdirectory(sysinfo)
add_subdirectory(translatorinspector)
add_subdirectory(webinspector)
add_subdirectory(wlcompositorinspector)


if(Qt5Gui_FOUND)
  add_subdirectory(guisupport)
  add_subdirectory(textdocumentinspector)
endif()

if(Qt5Widgets_FOUND)
  add_subdirectory(actioninspector)
  add_subdirectory(widgetinspector)
  add_subdirectory(sceneinspector)
  add_subdirectory(styleinspector)
endif()

# just checking whether Qt5::ScriptTools is found should be enough, but it isn't.
# thus check both that Qt5::Script & Qt5::ScriptTools are found to fix https://github.com/KDAB/GammaRay/issues/492
if((Qt5Script_FOUND AND Qt5ScriptTools_FOUND) OR (Qt5Script_FOUND AND Qt5ScriptTools_FOUND))
  add_subdirectory(scriptenginedebugger)
endif()

if(Qt5Qml_FOUND)
  add_subdirectory(qmlsupport)
endif()

if(Qt5QuickWidgets_FOUND)
  add_subdirectory(quickwidgetsupport)
endif()

if(Qt5Bluetooth_FOUND)
  add_subdirectory(bluetooth)
endif()

if(Qt5Positioning_FOUND)
  add_subdirectory(positioning)
endif()

if(Qt53DRender_FOUND AND Qt53DExtras_FOUND)
  add_subdirectory(qt3dinspector)
endif()

