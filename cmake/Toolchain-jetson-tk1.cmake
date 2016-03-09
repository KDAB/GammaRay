# Basic cmake toolchain file for Freescale iMX6
# Assumptions: toolchain is in path, $SYSROOT points to the sysroot
#

#  Copyright (c) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "armv7-a")
set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=$ENV{SYSROOT} -mtune=cortex-a15 -march=armv7-a -mfpu=neon-vfpv4")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=$ENV{SYSROOT} -mtune=cortex-a15 -march=armv7-a -mfpu=neon-vfpv4")
set(CMAKE_EXE_LINKER_FLAGS "--sysroot=$ENV{SYSROOT} -Wl,-rpath-link,$ENV{SYSROOT}/usr/lib/arm-linux-gnueabihf/tegra" CACHE STRING "executable linker flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

