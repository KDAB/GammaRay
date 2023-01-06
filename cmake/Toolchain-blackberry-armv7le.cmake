# Basic cmake toolchain file for BlackBerry 10

# SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Rafael Roquetto <rafael.roquetto@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

# the name of the target operating system
set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_SYSTEM_PROCESSOR "armv7le")

# which compilers to use for C and C++
set(arch gcc_ntoarmv7le)
set(CMAKE_C_COMPILER qcc -V${arch})
set(CMAKE_CXX_COMPILER QCC -V${arch})

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH $ENV{QNX_TARGET}/armle-v7 $ENV{QNX_TARGET})

if(CMAKE_HOST_WIN32)
    set(HOST_EXECUTABLE_SUFFIX ".exe")
endif()

set(CMAKE_AR
    "$ENV{QNX_HOST}/usr/bin/ntoarmv7-ar${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX ar Program"
)
set(CMAKE_RANLIB
    "$ENV{QNX_HOST}/usr/bin/ntoarmv7-ranlib${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX ar Program"
)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
