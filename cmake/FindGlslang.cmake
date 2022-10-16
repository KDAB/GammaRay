# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

find_program(GLSLANG_VALIDATOR_EXECUTABLE glslangValidator)
if(GLSLANG_VALIDATOR_EXECUTABLE)
    set(Glslang_FOUND TRUE)
endif()

# Copied from ECMPoQmTools which copied it from FindGettext.cmake
function(_glslang_get_unique_target_name _name _unique_name)
    set(propertyName "_GLSLANG_UNIQUE_COUNTER_${_name}")
    get_property(currentCounter GLOBAL PROPERTY "${propertyName}")
    if(NOT currentCounter)
        set(currentCounter 1)
    endif()
    set(${_unique_name}
        "${_name}_${currentCounter}"
        PARENT_SCOPE
    )
    math(EXPR currentCounter "${currentCounter} + 1")
    set_property(GLOBAL PROPERTY ${propertyName} ${currentCounter})
endfunction()

# validate individual shaders
function(glslang_validate_shader)
    if(NOT GLSLANG_VALIDATOR_EXECUTABLE)
        return()
    endif()

    _glslang_get_unique_target_name(_validate_shader _uid)
    foreach(_shader ${ARGN})
        get_filename_component(_shader_abs ${_shader} ABSOLUTE)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader
            COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} ${_shader_abs}
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader
            MAIN_DEPENDENCY ${_shader_abs}
            COMMENT "Generate a Validation Shader"
        )
        add_custom_target(
            ${_shader}${_uid} ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_shader}.validate_shader
            COMMENT "Target to generate a Validation Shader"
        )
    endforeach()

endfunction()

# validate an entire shader program, ie. also check for linker errors
function(glslang_validate_program)
    if(NOT GLSLANG_VALIDATOR_EXECUTABLE)
        return()
    endif()

    _glslang_get_unique_target_name(validate_program _uid)
    foreach(_shader ${ARGN})
        get_filename_component(_shader_abs ${_shader} ABSOLUTE)
        get_filename_component(_shader_name ${_shader} NAME)
        set(_target "${_target}${_shader_name}_")
        list(APPEND _shaders ${_shader_abs})
    endforeach()
    set(_target "${_target}${_uid}")

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp
        COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} -l ${_shaders}
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp
        DEPENDS ${_shaders}
        COMMENT "Generate a timestamp semaphore to associate with a Shader"
    )
    add_custom_target(
        ${_target} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_target}.stamp
        COMMENT "Target to generate the Shader timestamp semaphore"
    )
endfunction()
