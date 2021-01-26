#
# Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# All rights reserved.
#
# Author: Andras Mantia <andras.mantia@kdab.com>
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
# Creates targets for IFW packages.
#
# Usage:
#  addPackageTarget(package_name build_target install_target package_type)
#
# Where:
# - package_name is the name of the package (application), like GammaRay
# - build_target is the name of the make target used to build the required code (eg. src, sub-src, lib, docs, etc)
# - install_target is the name of the make target used to install the required code (eg. install, install_docs, etc)
# - package_type is the type of the package. Should be one of lib, doc, examples, tools
#
#
# For each type there will be a target created, eg. packagelib, packagedoc, etc. and there will be
# a master target called packages. Running
#
#     make packages
#
# will create the 7z packages for all added targets.

get_filename_component(PACKAGE_PREFIX ${QT_INSTALL_PREFIX} NAME)
set(INSTALL_PATH "${CMAKE_BINARY_DIR}/install/${QtCore_VERSION}/${PACKAGE_PREFIX}")
set(PACKAGE_PATH "${CMAKE_BINARY_DIR}/install/")

add_custom_target(packages)
# Used to serialize the package creation, otherwise the packaging for different types will
# conflict due to installing to the same place
set(LAST_TARGET "")

macro(addPackageTarget packageName dependsTarget installTarget type)
  set(PACKAGE_LIB_FILE "${CMAKE_BINARY_DIR}/${packageName}-${type}-${QtCore_VERSION}-${PACKAGE_PREFIX}.7z")
  set(PACKAGE_TARGET "${type}_package")
  add_custom_target(${PACKAGE_TARGET}
    DEPENDS "${LAST_TARGET}"
    COMMAND echo "Creating IFW package for ${packageName} of type ${type}: ${PACKAGE_LIB_FILE}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/${installTarget}"
    COMMAND ${CMAKE_COMMAND} -E remove -f ${PACKAGE_LIB_FILE}
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX="${INSTALL_PATH}" -DBUILD_TESTING=Off
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}" "${CMAKE_MAKE_PROGRAM}" ${installTarget}
    COMMAND ${CMAKE_COMMAND} -E chdir "${PACKAGE_PATH}" 7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on ${PACKAGE_LIB_FILE} .
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/${installTarget}"
    COMMAND echo "Generated package file: ${PACKAGE_LIB_FILE}"
  )
  add_dependencies(packages ${PACKAGE_TARGET})
  set(LAST_TARGET ${PACKAGE_TARGET})
endmacro()

