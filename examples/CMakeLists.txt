add_subdirectory(quick-batching)
add_subdirectory(quick-event-handling)
add_subdirectory(state-machine)

if(TARGET Qt::Widgets)
  add_subdirectory(signal-slot)
  add_subdirectory(timer)
  add_subdirectory(widget-layouting)
endif()

if(Qt53DExtras_FOUND)
  add_subdirectory(qt3d-geometry)
endif()
