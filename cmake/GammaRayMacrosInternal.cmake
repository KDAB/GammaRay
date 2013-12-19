# GammaRay-specific CMake macros that don't make sense outside of the GammaRay source tree.

#  Copyright (c) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

macro(gammaray_install_headers)
  get_filename_component(_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(FILES ${ARGN} DESTINATION ${INCLUDE_INSTALL_DIR}/${_dir})
  endif()
  set_directory_properties(PROPERTIES GAMMARAY_INSTALLED_HEADERS "${ARGN}")
endmacro()

macro(gammaray_all_installed_headers _var)
  set(${_var} "")
  foreach(_dir ${ARGN})
    get_directory_property(_hdrs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_dir} GAMMARAY_INSTALLED_HEADERS)
    foreach(_hdr ${_hdrs})
      list(APPEND ${_var} "${CMAKE_CURRENT_SOURCE_DIR}/${_dir}/${_hdr}")
    endforeach()
  endforeach()
endmacro()

macro(gammaray_join_list _var _sep)
  set(${_var} "")
  foreach(_element ${ARGN})
    set(${_var} "${${_var}}${_sep}${_element}")
  endforeach()
endmacro()
