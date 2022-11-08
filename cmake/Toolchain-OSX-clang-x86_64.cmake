# Toolchain file for building x86_64 binaries on OSX ARM-based systems
#

# SPDX-FileCopyrightText: 2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Allen Winter <allen.winter@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SYSTEM_NAME "Darwin")
set(CMAKE_SYSTEM_PROCESSOR "x86_64")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch x86_64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64")
