# Toolchain file for 32bit builds on 64bit hosts
#

#  Copyright (c) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_C_COMPILER_ARG1 "-m32")
set(CMAKE_CXX_COMPILER_ARG1 "-m32")
