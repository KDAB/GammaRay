#  Copyright (c) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

find_program(GLSLANG_VALIDATOR_EXECUTABLE glslangValidator)
if (GLSLANG_VALIDATOR_EXECUTABLE)
  set(Glslang_FOUND TRUE)
endif()

# validate individual shaders
function(glslang_validate_shader)
  if (NOT GLSLANG_VALIDATOR_EXECUTABLE)
    return()
  endif()

  foreach(_shader ${ARGN})
    get_filename_component(_shader_abs ${_shader} ABSOLUTE)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader
      COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} ${_shader_abs}
      COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader
      MAIN_DEPENDENCY ${_shader_abs}
    )
    add_custom_target(${_shader}_validate_shader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader)
  endforeach()

endfunction()

# validate an entire shader program, ie. also check for linker errors
function(glslang_validate_program)
  if (NOT GLSLANG_VALIDATOR_EXECUTABLE)
    return()
  endif()

  foreach(_shader ${ARGN})
    get_filename_component(_shader_abs ${_shader} ABSOLUTE)
    get_filename_component(_shader_name ${_shader} NAME)
    set(_target "${_target}${_shader_name}_")
    list(APPEND _shaders ${_shader_abs})
  endforeach()
  set(_target "${_target}validate_program")

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp
    COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} -l ${_shaders}
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp
    DEPENDS ${_shaders}
  )
  add_custom_target(${_target} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp)
endfunction()
