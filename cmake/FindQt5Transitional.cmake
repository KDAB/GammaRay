
find_package(Qt5Core QUIET)

if (Qt5Core_FOUND)
  if (NOT Qt5Transitional_FIND_COMPONENTS)
    foreach(_component
          Core
          Gui
          DBus
          Designer
          Script
          ScriptTools
          Network
          Test
          Xml
          Svg
          Sql
          Widgets
          PrintSupport
          Concurrent
          UiTools
          Quick1
          WebKit
          OpenGL
        )
      find_package(Qt5${_component})
    endforeach()
  else()
    foreach(_component ${Qt5Transitional_FIND_COMPONENTS})
      if ("${_component}" STREQUAL "Declarative")
        set(_component Quick1)
      endif()
      find_package(Qt5${_component} REQUIRED)
      if ("${_component}" STREQUAL "Gui")
        find_package(Qt5Widgets REQUIRED)
        find_package(Qt5PrintSupport REQUIRED)
        find_package(Qt5Svg REQUIRED)
      endif()
      if ("${_component}" STREQUAL "Core")
        find_package(Qt5Concurrent REQUIRED)
      endif()
    endforeach()
  endif()

  set(Qt5Transitional_FOUND TRUE)
  set(QT5_BUILD TRUE)

  include("${CMAKE_CURRENT_LIST_DIR}/ECMQt4To5Porting.cmake") # TODO: Port away from this.
  include_directories(${QT_INCLUDES}) # TODO: Port away from this.
else()
  foreach(_component ${Qt5Transitional_FIND_COMPONENTS})
    if("${_component}" STREQUAL "Widgets")  # new in Qt5
      set(_component Gui)
    elseif("${_component}" STREQUAL "Concurrent")   # new in Qt5
      set(_component Core)
    endif()
    list(APPEND _components Qt${_component})
  endforeach()
  find_package(Qt4 REQUIRED ${_components})
  include_directories(${QT_INCLUDES})

  if(QT4_FOUND)
    set(Qt5Transitional_FOUND TRUE)
  endif()
endif()
