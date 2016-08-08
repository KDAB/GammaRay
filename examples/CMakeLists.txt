if(Qt5Core_FOUND)
  add_subdirectory(quick-batching)
  add_subdirectory(quick-event-handling)
endif()

if(Qt5Widgets_FOUND)
  add_subdirectory(signal-slot)
endif()
