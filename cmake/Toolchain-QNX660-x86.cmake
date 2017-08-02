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

set(QNX_VERSION 6.6.0)
set(QNX_PROCESSOR x86)

set(QNX_HOST_HINT "C:/qnx660/host/win32/x86")
set(QNX_TARGET_HINT "C:/qnx660/target/qnx6")

set(CMAKE_C_FLAGS "-Vgcc_ntox86" CACHE STRING "qcc c flags" FORCE)
set(CMAKE_CXX_FLAGS "-Vgcc_ntox86 -lang-c++" CACHE STRING "qcc cxx flags" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/Toolchain-QNX660-common.cmake")
