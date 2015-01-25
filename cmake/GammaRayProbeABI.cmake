# Determine probe ABI identifier
#

#  Copyright (c) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

# Author: Volker Krause <volker.krause@kdab.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.

# This contains all properties that define ABI compatibility of a probe with a target

# Qt version
if(Qt5Core_FOUND)
  set(GAMMARAY_PROBE_ABI "qt${Qt5Core_VERSION_MAJOR}.${Qt5Core_VERSION_MINOR}")
else()
  set(GAMMARAY_PROBE_ABI "qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")
endif()

# on Windows, the compiler also matters
if(WIN32)
  set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_CXX_COMPILER_ID}")
endif()

# debug vs. release (MSVC only)
if(MSVC)
  if(CMAKE_BUILD_TYPE MATCHES "^[Rr]el")
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-release")
  else()
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-debug")
  endif()
endif()

# processor architecture
# this is a bit messy since CMAKE_SYSTEM_PROCESSOR seems to contain the host CPU rather than the target architecture sometimes
# and is empty on cross-builds by default
if(NOT CMAKE_SYSTEM_PROCESSOR)
  message(FATAL_ERROR "Unknown target architecture. Make sure to specify CMAKE_SYSTEM_PROCESSOR in your toolchain file!")
endif()

# on Windows our best bet is CMAKE_SIZEOF_VOID_P and assuming a x86 host==target build
if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
  else()
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-i686")
  endif()

# on Mac we apparently always get i386 on x86
elseif(APPLE)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-x86_64")
  else()
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_SYSTEM_PROCESSOR}")
  endif()

else()
  # uname reports different ARM versions, unlike ELF, so map all this to "arm"
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-arm")
  else()
    set(GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}-${CMAKE_SYSTEM_PROCESSOR}")
  endif()
endif()

message(STATUS "Building probe for ABI: ${GAMMARAY_PROBE_ABI}")
