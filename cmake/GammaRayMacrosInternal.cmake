# GammaRay-specific CMake macros that don't make sense outside of the GammaRay source tree.

#  Copyright (c) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

macro(gammaray_install_headers)
  get_filename_component(_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(FILES ${ARGN} DESTINATION ${INCLUDE_INSTALL_DIR}/${_dir})
  endif()
  set_directory_properties(PROPERTIES GAMMARAY_INSTALLED_HEADERS "${ARGN}")

  get_property(_include_dirs GLOBAL PROPERTY GAMMARAY_HEADER_DIRS)
  list(APPEND _include_dirs "${_dir}")
  set_property(GLOBAL PROPERTY GAMMARAY_HEADER_DIRS "${_include_dirs}")
endmacro()

macro(gammaray_all_installed_headers _var)
  set(${_var} "")
  get_property(_include_dirs GLOBAL PROPERTY GAMMARAY_HEADER_DIRS)
  foreach(_dir ${_include_dirs})
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

macro(gammaray_inverse_dir _var _prefix)
  file(RELATIVE_PATH ${_var} "${CMAKE_INSTALL_PREFIX}/${_prefix}" "${CMAKE_INSTALL_PREFIX}")
endmacro()

# embed an Info.plist file into a non-bundled Mac executable
macro(gammaray_embed_info_plist _target _plist)
  configure_file(${_plist} ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist)
  if(APPLE)
    set_target_properties(${_target} PROPERTIES LINK_FLAGS "-sectcreate __TEXT __info_plist ${CMAKE_CURRENT_BINARY_DIR}/${_target}_Info.plist")
  endif()
endmacro()
