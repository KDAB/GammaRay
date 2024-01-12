# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

add_subdirectory(injector)

set(gammaray_launcher_shared_srcs
    clientlauncher.cpp
    clientlauncher.h
    injector/abstractinjector.cpp
    injector/abstractinjector.h
    injector/injectorfactory.cpp
    injector/injectorfactory.h
    injector/processinjector.cpp
    injector/processinjector.h
    injector/styleinjector.cpp
    injector/styleinjector.h
    launcher.cpp
    launcher.h
    launcherfinder.cpp
    launcherfinder.h
    launchoptions.cpp
    launchoptions.h
    libraryutil.cpp
    libraryutil.h
    networkdiscoverymodel.cpp
    networkdiscoverymodel.h
    probeabi.cpp
    probeabi.h
    probeabidetector.cpp
    probeabidetector.h
    probefinder.cpp
    probefinder.h
    selftest.cpp
    selftest.h
)

if(WIN32)
    list(
        APPEND
        gammaray_launcher_shared_srcs
        injector/basicwindllinjector.cpp
        injector/basicwindllinjector.h
        injector/windllinjector.cpp
        injector/windllinjector.h
        pefile.cpp
        pefile.h
        probeabidetector_win.cpp
    )
else()
    list(
        APPEND
        gammaray_launcher_shared_srcs
        injector/debuggerinjector.cpp
        injector/debuggerinjector.h
        injector/gdbinjector.cpp
        injector/gdbinjector.h
        injector/lldbinjector.cpp
        injector/lldbinjector.h
        injector/preloadinjector.cpp
        injector/preloadinjector.h
    )
    if(APPLE)
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_mac.cpp)
    elseif(UNIX)
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_elf.cpp)
    else()
        list(APPEND gammaray_launcher_shared_srcs probeabidetector_dummy.cpp)
    endif()
endif()

add_library(
    gammaray_launcher SHARED
    ${gammaray_launcher_shared_srcs}
)
generate_export_header(gammaray_launcher)
# cmake-lint: disable=E1120
set_target_properties(gammaray_launcher PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
gammaray_set_rpath(gammaray_launcher ${LIB_INSTALL_DIR})
target_compile_features(gammaray_launcher PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

gammaray_target_relocatable_interfaces(gammaray_launcher_ipaths)
target_include_directories(
    gammaray_launcher
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_launcher_ipaths}>
)
target_link_libraries(
    gammaray_launcher
    PUBLIC Qt::Core
    PRIVATE gammaray_common Qt::Network ${CMAKE_DL_LIBS}
)
if(HAVE_QT_WIDGETS)
    target_link_libraries(gammaray_launcher PRIVATE Qt::Gui Qt::Widgets)
endif()
if(WIN32)
    target_link_libraries(gammaray_launcher PRIVATE version)
elseif(APPLE)
    target_link_libraries(gammaray_launcher PRIVATE "-framework CoreFoundation")
endif()

install(
    TARGETS gammaray_launcher
    EXPORT GammaRayTargets
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

gammaray_install_headers(
    DESTINATION
    launcher
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_launcher_export.h
    launcher.h
    launchoptions.h
    launcherfinder.h
    probeabi.h
    probeabidetector.h
    probefinder.h
    networkdiscoverymodel.h
)

ecm_generate_pri_file(
    BASE_NAME
    GammaRayLauncher
    LIB_NAME
    gammaray_launcher
    DEPS
    "core gui GammaRayCommon"
    FILENAME_VAR
    PRI_FILENAME
    INCLUDE_INSTALL_DIR
    ${INCLUDE_INSTALL_DIR}
)

install(
    FILES ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR}
)

if(MSVC)
    install(
        FILES "$<TARGET_PDB_FILE_DIR:gammaray_launcher>/$<TARGET_PDB_FILE_NAME:gammaray_launcher>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
