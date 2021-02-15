if (TARGET Qt5::Core)
    add_subdirectory(codecbrowser)
    add_subdirectory(statemachineviewer)
endif()
add_subdirectory(eventmonitor)
add_subdirectory(fontbrowser)
add_subdirectory(kjobtracker)
add_subdirectory(localeinspector)
add_subdirectory(modelinspector)
add_subdirectory(quickinspector)
add_subdirectory(signalmonitor)
add_subdirectory(timertop)

add_subdirectory(mimetypes)
add_subdirectory(network)
add_subdirectory(objectvisualizer)
add_subdirectory(qtivi)
add_subdirectory(sysinfo)
add_subdirectory(translatorinspector)
add_subdirectory(webinspector)
add_subdirectory(wlcompositorinspector)


if(TARGET Qt::Gui)
  add_subdirectory(guisupport)
  add_subdirectory(textdocumentinspector)
endif()
if(TARGET Qt5::Gui OR TARGET Qt6::OpenGL)
  add_subdirectory(openglsupport)
endif()

if(TARGET Qt::Widgets)
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

if(TARGET Qt::Qml)
  add_subdirectory(qmlsupport)
endif()

if(TARGET Qt::QuickWidgets)
  add_subdirectory(quickwidgetsupport)
endif()

if(TARGET Qt::Bluetooth)
  add_subdirectory(bluetooth)
endif()

if(TARGET Qt::Positioning)
  add_subdirectory(positioning)
endif()

if(TARGET Qt5::3DRender AND TARGET Qt5::3DExtras)
  add_subdirectory(qt3dinspector)
endif()

