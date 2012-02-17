
option(QT5_BUILD "Build with Qt5")

if (QT5_BUILD)
  if (NOT Qt5Transitional_FIND_COMPONENTS)
    foreach(_component
          Core
          Gui
          DBus
          DBusTools # For macros
          Designer
          Declarative
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
          Quick
          WebKit
        )
      find_package(Qt5${_component})
    endforeach()
  else()
    foreach(_component ${Qt5Transitional_FIND_COMPONENTS})
      find_package(Qt5${_component} REQUIRED)
      if ("${_component}" STREQUAL "Gui")
        find_package(Qt5Widgets REQUIRED)
        find_package(Qt5PrintSupport REQUIRED)
        find_package(Qt5Svg REQUIRED)
        find_package(Qt5Xml REQUIRED)
      endif()
      if ("${_component}" STREQUAL "DBus")
        find_package(Qt5DBusTools REQUIRED)
      endif()
      if ("${_component}" STREQUAL "Core")
        find_package(Qt5Concurrent REQUIRED)
      endif()
    endforeach()
  endif()
  include("${CMAKE_CURRENT_LIST_DIR}/ECMQt4To5Porting.cmake") # TODO: Port away from this.
  include_directories(${QT_INCLUDES}) # TODO: Port away from this.
else()
  foreach(_component ${Qt5Transitional_FIND_COMPONENTS})
    list(APPEND _components Qt${_component})
  endforeach()
  find_package(Qt4 REQUIRED ${_components})
  include_directories(${QT_INCLUDES})
  if(QT4_FOUND)
    set(Qt5Transitional_FOUND TRUE)
  endif()
endif()
