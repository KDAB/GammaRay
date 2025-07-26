# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

set(gammaray_srcs
    ${CMAKE_SOURCE_DIR}/3rdparty/qt/resourcemodel.cpp
    abstractbindingprovider.cpp
    abstractbindingprovider.h
    aggregatedpropertymodel.cpp
    aggregatedpropertymodel.h
    associativepropertyadaptor.cpp
    associativepropertyadaptor.h
    attributemodel.cpp
    attributemodel.h
    bindingaggregator.cpp
    bindingaggregator.h
    bindingnode.cpp
    bindingnode.h
    classesiconsrepositoryserver.cpp
    classesiconsrepositoryserver.h
    dynamicpropertyadaptor.cpp
    dynamicpropertyadaptor.h
    enumrepositoryserver.cpp
    enumrepositoryserver.h
    enumutil.cpp
    enumutil.h
    execution.cpp
    execution.h
    favoriteobject.cpp
    favoriteobject.h
    jsonpropertyadaptor.cpp
    jsonpropertyadaptor.h
    metaobject.cpp
    metaobject.h
    metaobjectregistry.cpp
    metaobjectregistry.h
    metaobjectrepository.cpp
    metaobjectrepository.h
    metaproperty.cpp
    metaproperty.h
    metapropertyadaptor.cpp
    metapropertyadaptor.h
    methodargumentmodel.cpp
    methodargumentmodel.h
    multisignalmapper.cpp
    multisignalmapper.h
    objectclassinfomodel.cpp
    objectclassinfomodel.h
    objectdataprovider.cpp
    objectdataprovider.h
    objectenummodel.cpp
    objectenummodel.h
    objectinstance.cpp
    objectinstance.h
    objectlistmodel.cpp
    objectlistmodel.h
    objectmethodmodel.cpp
    objectmethodmodel.h
    objecttreemodel.cpp
    objecttreemodel.h
    objecttypefilterproxymodel.cpp
    objecttypefilterproxymodel.h
    paintanalyzer.cpp
    paintanalyzer.h
    paintbuffer.cpp
    paintbuffer.h
    paintbuffermodel.cpp
    paintbuffermodel.h
    painterprofilingreplayer.cpp
    painterprofilingreplayer.h
    probe.cpp
    probe.h
    probecontroller.cpp
    probecontroller.h
    probeguard.cpp
    probeguard.h
    probesettings.cpp
    probesettings.h
    problemcollector.cpp
    problemcollector.h
    propertyadaptor.cpp
    propertyadaptor.h
    propertyadaptorfactory.cpp
    propertyadaptorfactory.h
    propertyaggregator.cpp
    propertyaggregator.h
    propertycontroller.cpp
    propertycontroller.h
    propertycontrollerextension.cpp
    propertycontrollerextension.h
    propertydata.cpp
    propertydata.h
    propertyfilter.cpp
    propertyfilter.h
    proxytoolfactory.cpp
    proxytoolfactory.h
    qmetaobjectvalidator.cpp
    qmetaobjectvalidator.h
    qmetapropertyadaptor.cpp
    qmetapropertyadaptor.h
    remote/localserverdevice.cpp
    remote/localserverdevice.h
    remote/remotemodelserver.cpp
    remote/remotemodelserver.h
    remote/selectionmodelserver.cpp
    remote/selectionmodelserver.h
    remote/server.cpp
    remote/server.h
    remote/serverdevice.cpp
    remote/serverdevice.h
    remote/serverproxymodel.cpp
    remote/serverproxymodel.h
    remote/tcpserverdevice.cpp
    remote/tcpserverdevice.h
    remoteviewserver.cpp
    remoteviewserver.h
    sequentialpropertyadaptor.cpp
    sequentialpropertyadaptor.h
    signalspycallbackset.cpp
    signalspycallbackset.h
    singlecolumnobjectproxymodel.cpp
    singlecolumnobjectproxymodel.h
    stacktracemodel.cpp
    stacktracemodel.h
    toolfactory.cpp
    toolfactory.h
    toolmanager.cpp
    toolmanager.h
    toolpluginerrormodel.cpp
    toolpluginerrormodel.h
    toolpluginmodel.cpp
    toolpluginmodel.h
    tools/messagehandler/messagehandler.cpp
    tools/messagehandler/messagehandler.h
    tools/messagehandler/messagemodel.cpp
    tools/messagehandler/messagemodel.h
    tools/metaobjectbrowser/metaobjectbrowser.cpp
    tools/metaobjectbrowser/metaobjectbrowser.h
    tools/metaobjectbrowser/metaobjecttreemodel.cpp
    tools/metaobjectbrowser/metaobjecttreemodel.h
    tools/metatypebrowser/metatypebrowser.cpp
    tools/metatypebrowser/metatypebrowser.h
    tools/metatypebrowser/metatypesmodel.cpp
    tools/metatypebrowser/metatypesmodel.h
    tools/objectinspector/abstractconnectionsmodel.cpp
    tools/objectinspector/abstractconnectionsmodel.h
    tools/objectinspector/applicationattributeextension.cpp
    tools/objectinspector/applicationattributeextension.h
    tools/objectinspector/bindingextension.cpp
    tools/objectinspector/bindingextension.h
    tools/objectinspector/bindingmodel.cpp
    tools/objectinspector/bindingmodel.h
    tools/objectinspector/classinfoextension.cpp
    tools/objectinspector/classinfoextension.h
    tools/objectinspector/connectionsextension.cpp
    tools/objectinspector/connectionsextension.h
    tools/objectinspector/enumsextension.cpp
    tools/objectinspector/enumsextension.h
    tools/objectinspector/inboundconnectionsmodel.cpp
    tools/objectinspector/inboundconnectionsmodel.h
    tools/objectinspector/methodsextension.cpp
    tools/objectinspector/methodsextension.h
    tools/objectinspector/objectinspector.cpp
    tools/objectinspector/objectinspector.h
    tools/objectinspector/outboundconnectionsmodel.cpp
    tools/objectinspector/outboundconnectionsmodel.h
    tools/objectinspector/propertiesextension.cpp
    tools/objectinspector/propertiesextension.h
    tools/objectinspector/stacktraceextension.cpp
    tools/objectinspector/stacktraceextension.h
    tools/problemreporter/availablecheckersmodel.cpp
    tools/problemreporter/availablecheckersmodel.h
    tools/problemreporter/problemmodel.cpp
    tools/problemreporter/problemmodel.h
    tools/problemreporter/problemreporter.cpp
    tools/problemreporter/problemreporter.h
    tools/resourcebrowser/resourcebrowser.cpp
    tools/resourcebrowser/resourcebrowser.h
    tools/resourcebrowser/resourcefiltermodel.cpp
    tools/resourcebrowser/resourcefiltermodel.h
    util.cpp
    util.h
    varianthandler.cpp
    varianthandler.h
    ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc
)

set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/loggingcategorymodel.cpp)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/)
list(APPEND gammaray_srcs ${CMAKE_SOURCE_DIR}/3rdparty/qt/5.5/private/qpaintbuffer.cpp)

if(WIN32 AND NOT MINGW)
    list(APPEND gammaray_srcs ${CMAKE_SOURCE_DIR}/3rdparty/StackWalker/StackWalker.cpp)
endif()

# core lib
add_library(
    gammaray_core
    ${GAMMARAY_LIBRARY_TYPE} ${gammaray_srcs}
)
generate_export_header(gammaray_core)
target_compile_features(gammaray_core PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

gammaray_target_relocatable_interfaces(gammaray_core_ipaths)
target_include_directories(
    gammaray_core
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_core_ipaths}>
)
target_include_directories(
    gammaray_core SYSTEM
    PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty/backward-cpp
)
target_link_libraries(
    gammaray_core
    PUBLIC gammaray_common
    PRIVATE $<BUILD_INTERFACE:gammaray_common_internal>
            gammaray_kitemmodels
            ${CMAKE_DL_LIBS}
            Qt::CorePrivate
            Qt::GuiPrivate
)
if(GAMMARAY_USE_PCH)
    target_precompile_headers(gammaray_core REUSE_FROM gammaray_pch_core_gui)
endif()

# cmake-lint: disable=E1120
set_target_properties(
    gammaray_core PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES} OUTPUT_NAME gammaray_core-${GAMMARAY_PROBE_ABI}
)
gammaray_set_rpath(gammaray_core ${LIB_INSTALL_DIR})

target_link_libraries(
    gammaray_core
    PUBLIC Qt::Core
    PRIVATE Qt::Gui Qt::GuiPrivate Qt::Network
)
if(TARGET Qt::AndroidExtras)
    target_link_libraries(gammaray_core PRIVATE Qt::AndroidExtras)
endif()

if(NOT (WIN32 AND NOT MSVC)) # not working in MinGW
    add_backward(gammaray_core)
endif()

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(
        TARGETS gammaray_core
        EXPORT GammaRayTargets
        ${INSTALL_TARGETS_DEFAULT_ARGS}
    )

    gammaray_install_headers(
        ${CMAKE_CURRENT_BINARY_DIR}/gammaray_core_export.h
        enumrepositoryserver.h
        enumutil.h
        metaobject.h
        metaobjectrepository.h
        metaproperty.h
        objectmodelbase.h
        objectdataprovider.h
        objecttypefilterproxymodel.h
        probe.h
        probecontroller.h
        propertycontroller.h
        propertycontrollerextension.h
        signalspycallbackset.h
        singlecolumnobjectproxymodel.h
        toolfactory.h
        typetraits.h
        util.h
        varianthandler.h
        remote/server.h
    )

    ecm_generate_pri_file(
        BASE_NAME
        GammaRayCore
        LIB_NAME
        gammaray_core-${GAMMARAY_PROBE_ABI}
        DEPS
        "core gui GammaRayCommon"
        FILENAME_VAR
        PRI_FILENAME
        INCLUDE_INSTALL_DIR
        ${INCLUDE_INSTALL_DIR}
    )

    install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
    if(GAMMARAY_STATIC_PROBE)
        gammaray_install_headers(staticprobe.h)
    endif()
else()
    install(TARGETS gammaray_core ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
if(MSVC)
    install(
        FILES "$<TARGET_PDB_FILE_DIR:gammaray_core>/$<TARGET_PDB_FILE_NAME:gammaray_core>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
