# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Volker Krause <volker.krause@kdab.com>
# Author: Filipe Azevedo <filipe.azevedo@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# This contains all properties that define ABI compatibility of a probe with a target

# Qt version
set(GAMMARAY_PROBE_ABI "qt${QT_VERSION_MAJOR}_${QtCore_VERSION_MINOR}")
if(NOT ANDROID)
    set(ANDROID_ABI "")
endif()

# on Windows, the compiler also matters
if(WIN32)
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_CXX_COMPILER_ID}")
    if(MSVC)
        # based on FindBoost.cmake
        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 20)
            message(FATAL_ERROR "Unsupported compiler version detected ${CMAKE_CXX_COMPILER_VERSION}")
        elseif(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
            set(GAMMARAY_COMPILER_VERSION "140")
        elseif(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18)
            set(GAMMARAY_COMPILER_VERSION "120")
        elseif(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
            set(GAMMARAY_COMPILER_VERSION "110")
        elseif(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16)
            set(GAMMARAY_COMPILER_VERSION "100")
        else()
            message(FATAL_ERROR "Unsupported compiler version detected, please use at least MSVC2010")
        endif()
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${GAMMARAY_COMPILER_VERSION}")
    endif()
endif()

# processor architecture

# on Windows our best bet is CMAKE_SIZEOF_VOID_P and assuming a x86 host==target build
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
    else()
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
    endif()

    # on Mac we apparently always get i386 on x86
elseif(APPLE)
    if(NOT CMAKE_SYSTEM_PROCESSOR)
        message(
            FATAL_ERROR
                "Unknown target architecture. Make sure to specify CMAKE_SYSTEM_PROCESSOR in your toolchain file!"
        )
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
    else()
        set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    # on Android we derive this from ANDROID_ABI
elseif(ANDROID OR CMAKE_SYSTEM_NAME MATCHES "Android")
    if(DEFINED ANDROID_ABI)
        if(ANDROID_ABI MATCHES "arm64")
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-aarch64")
        elseif(ANDROID_ABI MATCHES "arm")
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-arm")
        elseif(ANDROID_ABI STREQUAL "x86_64")
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
        elseif(ANDROID_ABI STREQUAL "x86")
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
        else()
            message(FATAL_ERROR "Unsupported Android ABI ${ANDROID_ABI}.")
        endif()
    else()
        message(FATAL_ERROR "Unknown target ABI. Make sure ANDROID_ABI is set!")
    endif()
else()
    if(NOT CMAKE_SYSTEM_PROCESSOR)
        message(
            FATAL_ERROR
                "Unknown target architecture. Make sure to specify CMAKE_SYSTEM_PROCESSOR in your toolchain file!"
        )
    endif()
    # uname reports different ARM versions, unlike ELF, so map all this to "arm"
    # also, there are Yocto supplied toolchain files out there, reporting "cortexa9[hf]-neon-..." or similar here
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm"
       OR CMAKE_SYSTEM_PROCESSOR MATCHES "cortex"
       OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64"
    )
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-aarch64")
        else()
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-arm")
        endif()
    else()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3456]86"
           OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86"
           OR CMAKE_SYSTEM_PROCESSOR MATCHES "corei"
           OR CMAKE_SYSTEM_PROCESSOR MATCHES "amd64"
        )
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
            else()
                set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
            endif()
        else()
            # subsequent code assumes there is no '-' in the architecture identifier
            string(REPLACE "-" "_" _clean_processor ${CMAKE_SYSTEM_PROCESSOR})
            set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${_clean_processor}")
        endif()
    endif()
endif()

message(STATUS "Building probe for ABI: ${GAMMARAY_PROBE_ABI} (${CMAKE_BUILD_TYPE})")
