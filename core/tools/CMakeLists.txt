include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Disabled in Qt5 - QFormBuilder include is not installed
if(NOT Qt5Core_FOUND)
  add_subdirectory(widgetinspector)
endif()
