# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Andras Mantia <andras.mantia@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

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

add_custom_target(packages COMMENT "Top-level target for generating the individual packages")
# Used to serialize the package creation, otherwise the packaging for different types will
# conflict due to installing to the same place
set(LAST_TARGET "")

# Add a target for building a package
macro(
    addPackageTarget
    packageName
    dependsTarget
    installTarget
    type
)
    set(PACKAGE_LIB_FILE "${CMAKE_BINARY_DIR}/${packageName}-${type}-${QtCore_VERSION}-${PACKAGE_PREFIX}.7z")
    set(PACKAGE_TARGET "${type}_package")
    add_custom_target(
        ${PACKAGE_TARGET}
        DEPENDS "${LAST_TARGET}"
        COMMAND echo "Creating IFW package for ${packageName} of type ${type}: ${PACKAGE_LIB_FILE}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/${installTarget}"
        COMMAND ${CMAKE_COMMAND} -E remove -f ${PACKAGE_LIB_FILE}
        COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX="${INSTALL_PATH}" -DBUILD_TESTING=Off
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}" "${CMAKE_MAKE_PROGRAM}" ${installTarget}
        COMMAND ${CMAKE_COMMAND} -E chdir "${PACKAGE_PATH}" 7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on
                ${PACKAGE_LIB_FILE} .
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/${installTarget}"
        COMMAND echo "Generated package file: ${PACKAGE_LIB_FILE}"
        COMMENT "Generate a package file for the specified package"
    )
    add_dependencies(packages ${PACKAGE_TARGET})
    set(LAST_TARGET ${PACKAGE_TARGET})
endmacro()
