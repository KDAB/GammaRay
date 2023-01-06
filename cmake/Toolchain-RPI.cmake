# Basic cmake toolchain file for RaspberryPi
# Assumptions: toolchain is in path, $SYSROOT points to the sysroot
#

# SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "armv6l")
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
