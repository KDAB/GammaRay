# Basic cmake toolchain file for Freescale iMX6
# Assumptions: toolchain is in path, $SYSROOT points to the sysroot
#

#  Copyright (c) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "armv7-a")
set(CMAKE_C_COMPILER "arm-linux-gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=$ENV{SYSROOT} -march=armv7-a -mfpu=neon")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=$ENV{SYSROOT} -march=armv7-a -mfpu=neon")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "$ENV{SYSROOT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

