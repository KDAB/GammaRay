# Basic cmake toolchain file for Qt for Yocto Environment
# Assumptions: toolchain script is sourced
#

# SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Christoph Sterz <christoph.sterz@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SYSTEM_NAME "Linux")

if(DEFINED ENV{ARCH})
    #$ARCH is set through the yocto environment script, use this
    set(CMAKE_SYSTEM_PROCESSOR "$ENV{ARCH}")
elseif(DEFINED ENV{CC})
    #No $ARCH found, trying to deduce processor from -march=<name> flag in CC
    string(REGEX MATCH "-march=([^\ ]+)" DUMMY "$ENV{CC}")
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_MATCH_1})
else()
    message(FATAL_ERROR "Could not find processor architecture (no ARCH or CC found in environment).")
endif()

set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS "$ENV{OE_QMAKE_PATH_HOST_BINS}")
set(CMAKE_FIND_ROOT_PATH "$ENV{SDKTARGETSYSROOT}")
set(CMAKE_SYSROOT "$ENV{SDKTARGETSYSROOT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
