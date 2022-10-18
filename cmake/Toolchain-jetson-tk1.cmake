# Basic cmake toolchain file for Freescale iMX6
# Assumptions: toolchain is in path, $SYSROOT points to the sysroot
#

# SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "armv7-a")
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=cortex-a15 -march=armv7-a -mfpu=neon-vfpv4")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=cortex-a15 -march=armv7-a -mfpu=neon-vfpv4")
set(CMAKE_EXE_LINKER_FLAGS
    "--sysroot=$ENV{SYSROOT} -Wl,-rpath-link,$ENV{SYSROOT}/usr/lib/arm-linux-gnueabihf/tegra"
    CACHE STRING "executable linker flags" FORCE
)
set(CMAKE_SYSROOT "$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
