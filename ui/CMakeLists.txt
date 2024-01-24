# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# Shared code between in-process and out-of-process UI
#
set(gammaray_ui_srcs
    aboutdata.cpp
    aboutdata.h
    aboutwidget.cpp
    aboutwidget.h
    clientdecorationidentityproxymodel.cpp
    clientdecorationidentityproxymodel.h
    clientpropertymodel.cpp
    clientpropertymodel.h
    clienttoolmanager.cpp
    clienttoolmanager.h
    clienttoolmodel.cpp
    clienttoolmodel.h
    codeeditor/codeeditor.cpp
    codeeditor/codeeditor.h
    codeeditor/codeeditorsidebar.cpp
    codeeditor/codeeditorsidebar.h
    contextmenuextension.cpp
    contextmenuextension.h
    deferredtreeview.cpp
    deferredtreeview.h
    editabletypesmodel.cpp
    editabletypesmodel.h
    favoritesitemview.cpp
    favoritesitemview.h
    helpcontroller.cpp
    helpcontroller.h
    itemdelegate.cpp
    itemdelegate.h
    methodinvocationdialog.cpp
    methodinvocationdialog.h
    modelpickerdialog.cpp
    modelpickerdialog.h
    paintanalyzerreplayview.cpp
    paintanalyzerreplayview.h
    paintanalyzerwidget.cpp
    paintanalyzerwidget.h
    paintbufferclientmodel.cpp
    paintbufferclientmodel.h
    paintbufferviewer.cpp
    paintbufferviewer.h
    palettemodel.cpp
    palettemodel.h
    propertybinder.cpp
    propertybinder.h
    propertyeditor/palettedialog.cpp
    propertyeditor/palettedialog.h
    propertyeditor/propertycoloreditor.cpp
    propertyeditor/propertycoloreditor.h
    propertyeditor/propertydoublepaireditor.cpp
    propertyeditor/propertydoublepaireditor.h
    propertyeditor/propertyeditordelegate.cpp
    propertyeditor/propertyeditordelegate.h
    propertyeditor/propertyeditorfactory.cpp
    propertyeditor/propertyeditorfactory.h
    propertyeditor/propertyenumeditor.cpp
    propertyeditor/propertyenumeditor.h
    propertyeditor/propertyextendededitor.cpp
    propertyeditor/propertyextendededitor.h
    propertyeditor/propertyfonteditor.cpp
    propertyeditor/propertyfonteditor.h
    propertyeditor/propertyintpaireditor.cpp
    propertyeditor/propertyintpaireditor.h
    propertyeditor/propertymargineditor.cpp
    propertyeditor/propertymargineditor.h
    propertyeditor/propertymatrixdialog.cpp
    propertyeditor/propertymatrixdialog.h
    propertyeditor/propertymatrixeditor.cpp
    propertyeditor/propertymatrixeditor.h
    propertyeditor/propertymatrixmodel.cpp
    propertyeditor/propertymatrixmodel.h
    propertyeditor/propertypaletteeditor.cpp
    propertyeditor/propertypaletteeditor.h
    propertyeditor/propertyrecteditor.cpp
    propertyeditor/propertyrecteditor.h
    propertyeditor/propertytexteditor.cpp
    propertyeditor/propertytexteditor.h
    propertywidget.cpp
    propertywidget.h
    propertywidgettab.cpp
    propertywidgettab.h
    proxytooluifactory.cpp
    proxytooluifactory.h
    remoteviewwidget.cpp
    remoteviewwidget.h
    searchlinecontroller.cpp
    searchlinecontroller.h
    splashscreen.cpp
    splashscreen.h
    themedimagelabel.cpp
    themedimagelabel.h
    tools/messagehandler/messagedisplaymodel.cpp
    tools/messagehandler/messagedisplaymodel.h
    tools/messagehandler/messagehandlerclient.cpp
    tools/messagehandler/messagehandlerclient.h
    tools/messagehandler/messagehandlerwidget.cpp
    tools/messagehandler/messagehandlerwidget.h
    tools/metaobjectbrowser/metaobjectbrowserwidget.cpp
    tools/metaobjectbrowser/metaobjectbrowserwidget.h
    tools/metaobjectbrowser/metaobjecttreeclientproxymodel.cpp
    tools/metaobjectbrowser/metaobjecttreeclientproxymodel.h
    tools/metatypebrowser/metatypebrowserclient.cpp
    tools/metatypebrowser/metatypebrowserclient.h
    tools/metatypebrowser/metatypebrowserwidget.cpp
    tools/metatypebrowser/metatypebrowserwidget.h
    tools/metatypebrowser/metatypesclientmodel.cpp
    tools/metatypebrowser/metatypesclientmodel.h
    tools/objectinspector/applicationattributetab.cpp
    tools/objectinspector/applicationattributetab.h
    tools/objectinspector/bindingtab.cpp
    tools/objectinspector/bindingtab.h
    tools/objectinspector/classinfotab.cpp
    tools/objectinspector/classinfotab.h
    tools/objectinspector/clientmethodmodel.cpp
    tools/objectinspector/clientmethodmodel.h
    tools/objectinspector/connectionsclientproxymodel.cpp
    tools/objectinspector/connectionsclientproxymodel.h
    tools/objectinspector/connectionsextensionclient.cpp
    tools/objectinspector/connectionsextensionclient.h
    tools/objectinspector/connectionstab.cpp
    tools/objectinspector/connectionstab.h
    tools/objectinspector/enumstab.cpp
    tools/objectinspector/enumstab.h
    tools/objectinspector/methodsextensionclient.cpp
    tools/objectinspector/methodsextensionclient.h
    tools/objectinspector/methodstab.cpp
    tools/objectinspector/methodstab.cpp
    tools/objectinspector/methodstab.h
    tools/objectinspector/methodstab.h
    tools/objectinspector/objectinspectorwidget.cpp
    tools/objectinspector/objectinspectorwidget.h
    tools/objectinspector/propertiesextensionclient.cpp
    tools/objectinspector/propertiesextensionclient.h
    tools/objectinspector/propertiestab.cpp
    tools/objectinspector/propertiestab.h
    tools/objectinspector/stacktracetab.cpp
    tools/objectinspector/stacktracetab.h
    tools/problemreporter/problemclientmodel.cpp
    tools/problemreporter/problemclientmodel.h
    tools/problemreporter/problemreporterclient.cpp
    tools/problemreporter/problemreporterclient.h
    tools/problemreporter/problemreporterwidget.cpp
    tools/problemreporter/problemreporterwidget.h
    tools/resourcebrowser/clientresourcemodel.cpp
    tools/resourcebrowser/clientresourcemodel.h
    tools/resourcebrowser/resourcebrowserclient.cpp
    tools/resourcebrowser/resourcebrowserclient.h
    tools/resourcebrowser/resourcebrowserwidget.cpp
    tools/resourcebrowser/resourcebrowserwidget.h
    tooluifactory.cpp
    tooluifactory.h
    trailingcolorlabel.cpp
    trailingcolorlabel.h
    uiintegration.cpp
    uiintegration.h
    uiresources.cpp
    uiresources.h
    uistatemanager.cpp
    uistatemanager.h
    visibilityfilterproxymodel.cpp
    visibilityfilterproxymodel.h
    resources/ui.qrc
    ${CMAKE_CURRENT_BINARY_DIR}/resources/classicons.qrc
)

add_library(
    gammaray_ui SHARED
    ${gammaray_ui_srcs}
)
generate_export_header(gammaray_ui)
# cmake-lint: disable=E1120
set_target_properties(
    gammaray_ui PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES} OUTPUT_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
)
gammaray_set_rpath(gammaray_ui ${LIB_INSTALL_DIR})
target_compile_features(gammaray_ui PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

gammaray_target_relocatable_interfaces(gammaray_ui_ipaths)
target_include_directories(
    gammaray_ui
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_ui_ipaths}>
)

target_link_libraries(
    gammaray_ui
    PUBLIC Qt::Core Qt::Gui Qt::Widgets gammaray_common
    PRIVATE $<BUILD_INTERFACE:gammaray_common_internal> gammaray_kitemmodels Qt::WidgetsPrivate
)
if(TARGET KF${QtCore_VERSION_MAJOR}::SyntaxHighlighting)
    target_link_libraries(gammaray_ui PRIVATE KF${QtCore_VERSION_MAJOR}::SyntaxHighlighting)
endif()

set(gammaray_ui_internal_srcs
    aboutdialog.cpp
    aboutdialog.h
    aboutpluginsdialog.cpp
    aboutpluginsdialog.h
    clienttoolfilterproxymodel.cpp
    clienttoolfilterproxymodel.h
    mainwindow.cpp
    mainwindow.h
    sidepane.cpp
    sidepane.h
)

add_library(
    gammaray_ui_internal STATIC
    ${gammaray_ui_internal_srcs}
)
set_target_properties(gammaray_ui_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_ui_internal PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})
target_link_libraries(
    gammaray_ui_internal
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::GuiPrivate
    $<BUILD_INTERFACE:gammaray_common_internal>
    gammaray_ui
    gammaray_kitemmodels
)

if(NOT GAMMARAY_DISABLE_FEEDBACK)
    target_link_libraries(gammaray_ui_internal gammaray_kuserfeedback)
endif()

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(
        TARGETS gammaray_ui
        EXPORT GammaRayTargets
        ${INSTALL_TARGETS_DEFAULT_ARGS}
    )

    gammaray_install_headers(
        ${CMAKE_CURRENT_BINARY_DIR}/gammaray_ui_export.h
        clientdecorationidentityproxymodel.h
        clienttoolmanager.h
        propertyeditor/propertyeditordelegate.h
        propertywidget.h
        propertywidgettab.h
        tooluifactory.h
        uiintegration.h
        uistatemanager.h
        uiresources.h
    )

    ecm_generate_pri_file(
        BASE_NAME
        GammaRayUi
        LIB_NAME
        gammaray_ui-${GAMMARAY_PROBE_ABI}
        DEPS
        "core gui widgets GammaRayCommon"
        FILENAME_VAR
        PRI_FILENAME
        INCLUDE_INSTALL_DIR
        ${INCLUDE_INSTALL_DIR}
    )

    install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
else()
    install(TARGETS gammaray_ui ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
if(MSVC)
    install(
        FILES "$<TARGET_PDB_FILE_DIR:gammaray_ui>/$<TARGET_PDB_FILE_NAME:gammaray_ui>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
add_subdirectory(resources)
