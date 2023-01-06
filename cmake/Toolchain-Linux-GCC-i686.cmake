# Toolchain file for 32bit builds on 64bit Linux hosts
#

# SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
# Author: Filipe Azevedo <filipe.azevedo@kdab.com>

# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
