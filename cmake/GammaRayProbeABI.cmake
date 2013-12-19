# Determine probe ABI identifier
#
# This contains all properties that define ABI compatibility of a probe with a target
# It includes:
# - Qt version (major/minor)
# - Compiler (Windows only)
# - Release/Debug (Windows only) TODO
# - processor architecture

if(WIN32)
set(GAMMARAY_PROBE_ABI "qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}")
else()
set(GAMMARAY_PROBE_ABI "qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "Building probe for ABI: ${GAMMARAY_PROBE_ABI}")
