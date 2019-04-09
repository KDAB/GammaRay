# Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# All rights reserved.
#
# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_program(QMLLINT_EXECUTABLE qmllint)
if (QMLLINT_EXECUTABLE)
  if(NOT QMLLINT_IS_WORKING)
    # Try to fix common problems on Debian-based distros -- they provide /usr/bin/qmllint, which is a symlink to
    # /usr/lib/x86_64-linux-gnu/qt4/bin/qmllint (or the Qt5 version of it). The actual executable is part of different
    # package, so might not even be installed => double-check whether qmllint is working by executing it
    execute_process(COMMAND ${QMLLINT_EXECUTABLE} --version RESULT_VARIABLE _qmllint_result OUTPUT_QUIET ERROR_QUIET)
    if (_qmllint_result EQUAL 0)
      set(QMLLINT_IS_WORKING TRUE CACHE BOOL "Whether the found qmllint executable is actually usable" FORCE)
    endif()
  endif()
  if(QMLLINT_IS_WORKING)
    set(QmlLint_FOUND TRUE)
  endif()
endif()

# validate a list of qml files
function(qml_lint)
  if (NOT QMLLINT_EXECUTABLE OR NOT QmlLint_FOUND)
    return()
  endif()

  foreach(_file ${ARGN})
    get_filename_component(_file_abs ${_file} ABSOLUTE)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint
      COMMAND ${QMLLINT_EXECUTABLE} ${_file_abs}
      COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint
      MAIN_DEPENDENCY ${_file_abs}
    )
    add_custom_target(${_file}_qmllint ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_file}.qmllint)
  endforeach()
endfunction()
