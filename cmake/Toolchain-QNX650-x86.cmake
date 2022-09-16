#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_SYSTEM_VERSION 6.5.0)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(TOOLCHAIN QNX)

#SET(CMAKE_IMPORT_LIBRARY_SUFFIX ".a")

set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

if(CMAKE_HOST_WIN32)
    set(HOST_EXECUTABLE_SUFFIX ".exe")
endif(CMAKE_HOST_WIN32)

find_path(
    QNX_HOST NAME usr/bin/qcc${HOST_EXECUTABLE_SUFFIX}
    PATHS $ENV{QNX_HOST} C:/QNX650/host/win32/x86
    NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH
)

find_path(
    QNX_TARGET NAME usr/include/qnx_errno.h
    PATHS $ENV{QNX_TARGET} C:/QNX650/target/qnx6
    NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH
)

if(CMAKE_HOST_WIN32)
    find_path(
        QNX_CONFIGURATION NAME bin/qnxactivate.exe
        PATHS $ENV{QNX_CONFIGURATION} "C:/Program Files/QNX Software Systems/qconfig"
        NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH
    )
endif(CMAKE_HOST_WIN32)

set(ENV{QNX_HOST} ${QNX_HOST})
set(ENV{QNX_TARGET} ${QNX_TARGET})
if(CMAKE_HOST_WIN32)
    set(ENV{QNX_CONFIGURATION} ${QNX_CONFIGURATION})
    set(ENV{PATH} "$ENV{PATH};${QNX_HOST}/usr/bin")
endif(CMAKE_HOST_WIN32)

set(CMAKE_MAKE_PROGRAM
    "${QNX_HOST}/usr/bin/make${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX Make Program"
)
set(CMAKE_SH
    "${QNX_HOST}/usr/bin/sh${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX shell Program"
)
set(CMAKE_AR
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ar${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX ar Program"
)
set(CMAKE_RANLIB
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ranlib${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX ranlib Program"
)
set(CMAKE_NM
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-nm${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX nm Program"
)
set(CMAKE_OBJCOPY
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objcopy${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX objcopy Program"
)
set(CMAKE_OBJDUMP
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objdump${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX objdump Program"
)
set(CMAKE_LINKER
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ld"
    CACHE PATH "QNX Linker Program"
)
set(CMAKE_STRIP
    "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-strip${HOST_EXECUTABLE_SUFFIX}"
    CACHE PATH "QNX Strip Program"
)

set(CMAKE_C_COMPILER ${QNX_HOST}/usr/bin/qcc${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_CXX_COMPILER ${QNX_HOST}/usr/bin/qcc${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_FIND_ROOT_PATH ${QNX_TARGET})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS
    "-Vgcc_ntox86"
    CACHE STRING "qcc c flags" FORCE
)
set(CMAKE_CXX_FLAGS
    "-Vgcc_ntox86 -lang-c++"
    CACHE STRING "qcc cxx flags" FORCE
)
