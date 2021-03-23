# Determine probe ABI identifier
#

# Copyright (c) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# All rights reserved.
#
# Author: Volker Krause <volker.krause@kdab.com>
# Author: Filipe Azevedo <filipe.azevedo@kdab.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# This contains all properties that define ABI compatibility of a probe with a target

# Qt version
set(GAMMARAY_PROBE_ABI "qt${QtCore_VERSION_MAJOR}_${QtCore_VERSION_MINOR}")

# on Windows, the compiler also matters
if(WIN32)
  set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_CXX_COMPILER_ID}")
  if(MSVC)
      # based on FindBoost.cmake
      if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 20)
          message(FATAL_ERROR "Unsupported compiler version detected ${CMAKE_CXX_COMPILER_VERSION}")
      elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
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
    message(FATAL_ERROR "Unknown target architecture. Make sure to specify CMAKE_SYSTEM_PROCESSOR in your toolchain file!")
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
  if(NOT ANDROID_ABI)
    message(FATAL_ERROR "Unknown target ABI. Make sure ANDROID_ABI is set!")
  endif()
  if(ANDROID_ABI MATCHES "arm64")
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-aarch64")
  elseif(ANDROID_ABI MATCHES "arm" )
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-arm")
  elseif(ANDROID_ABI STREQUAL "x86_64" )
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
  elseif(ANDROID_ABI STREQUAL "x86" )
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
  else()
    message(FATAL_ERROR "Unsupported Android ABI ${ANDROID_ABI}.")
  endif()

else()
  if(NOT CMAKE_SYSTEM_PROCESSOR)
    message(FATAL_ERROR "Unknown target architecture. Make sure to specify CMAKE_SYSTEM_PROCESSOR in your toolchain file!")
  endif()
  # uname reports different ARM versions, unlike ELF, so map all this to "arm"
  # also, there are Yocto supplied toolchain files out there, reporting "cortexa9[hf]-neon-..." or similar here
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "cortex" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-aarch64")
    else()
      set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-arm")
    endif()
  else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3456]86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "corei" OR CMAKE_SYSTEM_PROCESSOR MATCHES "amd64")
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
