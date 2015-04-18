#.rst:
# ECMGeneratePriFile
# ------------------
#
# Generate a ``.pri`` file for the benefit of qmake-based projects.
#
# As well as the function below, this module creates the cache variable
# ``ECM_MKSPECS_INSTALL_DIR`` and sets the default value to ``mkspecs/modules``.
# This assumes Qt and the current project are both installed to the same
# non-system prefix.  Packagers who use ``-DCMAKE_INSTALL_PREFIX=/usr`` will
# certainly want to set ``ECM_MKSPECS_INSTALL_DIR`` to something like
# ``share/qt5/mkspecs/modules``.
#
# The main thing is that this should be the ``modules`` subdirectory of either
# the default qmake ``mkspecs`` directory or of a directory that will be in the
# ``$QMAKEPATH`` environment variable when qmake is run.
#
# ::
#
#   ecm_generate_pri_file(BASE_NAME <baseName>
#                         LIB_NAME <libName>
#                         [DEPS "<dep> [<dep> [...]]"]
#                         [FILENAME_VAR <filename_variable>]
#                         [INCLUDE_INSTALL_DIR <dir>]
#                         [LIB_INSTALL_DIR <dir>])
#
# If your CMake project produces a Qt-based library, you may expect there to be
# applications that wish to use it that use a qmake-based build system, rather
# than a CMake-based one.  Creating a ``.pri`` file will make use of your
# library convenient for them, in much the same way that CMake config files make
# things convenient for CMake-based applications.
#
# ecm_generate_pri_file() generates just such a file.  It requires the
# ``PROJECT_VERSION_STRING`` variable to be set.  This is typically set by
# :module:`ECMSetupVersion`, although the project() command in CMake 3.0.0 and
# later can also set this.
#
# BASE_NAME specifies the name qmake project (.pro) files should use to refer to
# the library (eg: KArchive).  LIB_NAME is the name of the actual library to
# link to (ie: the first argument to add_library()).  DEPS is a space-separated
# list of the base names of other libraries (for Qt libraries, use the same
# names you use with the ``QT`` variable in a qmake project file, such as "core"
# for QtCore).  FILENAME_VAR specifies the name of a variable to store the path
# to the generated file in.
#
# INCLUDE_INSTALL_DIR is the path (relative to ``CMAKE_INSTALL_PREFIX``) that
# include files will be installed to. It defaults to
# ``${INCLUDE_INSTALL_DIR}/<baseName>`` if the ``INCLUDE_INSTALL_DIR`` variable
# is set. If that variable is not set, the ``CMAKE_INSTALL_INCLUDEDIR`` variable
# is used instead, and if neither are set ``include`` is used.  LIB_INSTALL_DIR
# operates similarly for the installation location for libraries; it defaults to
# ``${LIB_INSTALL_DIR}``, ``${CMAKE_INSTALL_LIBDIR}`` or ``lib``, in that order.
#
# Example usage:
#
# .. code-block:: cmake
#
#   ecm_generate_pri_file(
#       BASE_NAME KArchive
#       LIB_NAME KF5KArchive
#       DEPS "core"
#       FILENAME_VAR pri_filename
#   )
#   install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
#
# A qmake-based project that wished to use this would then do::
#
#   QT += KArchive
#
# in their ``.pro`` file.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2014 David Faure <faure@kde.org>
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
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

if(KDE_INSTALL_USE_QT_SYS_PATHS)
  include(ECMQueryQmake)
  query_qmake(qt_host_data_dir QT_HOST_DATA)
  set(ECM_MKSPECS_INSTALL_DIR ${qt_host_data_dir}/mkspecs/modules CACHE PATH "The directory where mkspecs will be installed to.")
else ()
  set(ECM_MKSPECS_INSTALL_DIR mkspecs/modules CACHE PATH "The directory where mkspecs will be installed to.")
endif()

function(ECM_GENERATE_PRI_FILE)
  set(options )
  set(oneValueArgs BASE_NAME LIB_NAME DEPS FILENAME_VAR INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
  set(multiValueArgs )

  cmake_parse_arguments(EGPF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(EGPF_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to ECM_GENERATE_PRI_FILE(): \"${EGPF_UNPARSED_ARGUMENTS}\"")
  endif()

  if(NOT EGPF_BASE_NAME)
    message(FATAL_ERROR "Required argument BASE_NAME missing in ECM_GENERATE_PRI_FILE() call")
  endif()
  if(NOT EGPF_LIB_NAME)
    message(FATAL_ERROR "Required argument LIB_NAME missing in ECM_GENERATE_PRI_FILE() call")
  endif()
  if(NOT PROJECT_VERSION_STRING)
    message(FATAL_ERROR "Required variable PROJECT_VERSION_STRING not set before ECM_GENERATE_PRI_FILE() call. Did you call ecm_setup_version?")
  endif()
  if(NOT EGPF_INCLUDE_INSTALL_DIR)
      if(INCLUDE_INSTALL_DIR)
          set(EGPF_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}/${EGPF_BASE_NAME}")
      elseif(CMAKE_INSTALL_INCLUDEDIR)
          set(EGPF_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${EGPF_BASE_NAME}")
      else()
          set(EGPF_INCLUDE_INSTALL_DIR "include/${EGPF_BASE_NAME}")
      endif()
  endif()
  if(NOT EGPF_LIB_INSTALL_DIR)
      if(LIB_INSTALL_DIR)
          set(EGPF_LIB_INSTALL_DIR "${LIB_INSTALL_DIR}")
      elseif(CMAKE_INSTALL_LIBDIR)
          set(EGPF_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
      else()
          set(EGPF_LIB_INSTALL_DIR "lib")
      endif()
  endif()

  string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" PROJECT_VERSION_MAJOR "${PROJECT_VERSION_STRING}")
  string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" PROJECT_VERSION_MINOR "${PROJECT_VERSION_STRING}")
  string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_PATCH "${PROJECT_VERSION_STRING}")

  set(PRI_TARGET_BASENAME ${EGPF_BASE_NAME})
  set(PRI_TARGET_LIBNAME ${EGPF_LIB_NAME})
  set(PRI_TARGET_QTDEPS ${EGPF_DEPS})
  if(IS_ABSOLUTE "${EGPF_INCLUDE_INSTALL_DIR}")
      set(PRI_TARGET_INCLUDES "${EGPF_INCLUDE_INSTALL_DIR}")
  else()
      set(PRI_TARGET_INCLUDES "${CMAKE_INSTALL_PREFIX}/${EGPF_INCLUDE_INSTALL_DIR}")
  endif()
  if(IS_ABSOLUTE "${EGPF_LIB_INSTALL_DIR}")
      set(PRI_TARGET_LIBS "${EGPF_LIB_INSTALL_DIR}")
  else()
      set(PRI_TARGET_LIBS "${CMAKE_INSTALL_PREFIX}/${EGPF_LIB_INSTALL_DIR}")
  endif()
  set(PRI_TARGET_DEFINES "")

  set(PRI_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/qt_${PRI_TARGET_BASENAME}.pri)
  if (EGPF_FILENAME_VAR)
     set(${EGPF_FILENAME_VAR} ${PRI_FILENAME} PARENT_SCOPE)
  endif()

  file(GENERATE
     OUTPUT ${PRI_FILENAME}
     CONTENT
     "QT.${PRI_TARGET_BASENAME}.VERSION = ${PROJECT_VERSION_STRING}
QT.${PRI_TARGET_BASENAME}.MAJOR_VERSION = ${PROJECT_VERSION_MAJOR}
QT.${PRI_TARGET_BASENAME}.MINOR_VERSION = ${PROJECT_VERSION_MINOR}
QT.${PRI_TARGET_BASENAME}.PATCH_VERSION = ${PROJECT_VERSION_PATCH}
QT.${PRI_TARGET_BASENAME}.name = ${PRI_TARGET_LIBNAME}
QT.${PRI_TARGET_BASENAME}.defines = ${PRI_TARGET_DEFINES}
QT.${PRI_TARGET_BASENAME}.includes = ${PRI_TARGET_INCLUDES}
QT.${PRI_TARGET_BASENAME}.private_includes =
QT.${PRI_TARGET_BASENAME}.libs = ${PRI_TARGET_LIBS}
QT.${PRI_TARGET_BASENAME}.depends = ${PRI_TARGET_QTDEPS}
"
  )
endfunction()

