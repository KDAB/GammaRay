#=============================================================================
# Copyright 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Stephen Kelly <stephen.kelly@kdab.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Kitware, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

find_package(Qt5Core QUIET)

if (Qt5Core_FOUND)
  if (NOT Qt5Transitional_FIND_COMPONENTS)
    foreach(_component
          Core
          Gui
          DBus
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
          WebKit
          OpenGL
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
  find_package(Qt4 ${QT_MIN_VERSION} REQUIRED ${_components})
  include_directories(${QT_INCLUDES})

  if(QT4_FOUND)
    set(Qt5Transitional_FOUND TRUE)
  endif()
endif()
