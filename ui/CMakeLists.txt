#
# Shared code between in-process and out-of-process UI
#
set(gammaray_ui_srcs
  aboutdata.cpp
  aboutwidget.cpp
  themedimagelabel.cpp
  clientdecorationidentityproxymodel.cpp
  clientpropertymodel.cpp
  clienttoolmanager.cpp
  clienttoolmodel.cpp
  contextmenuextension.cpp
  deferredtreeview.cpp
  editabletypesmodel.cpp
  itemdelegate.cpp
  methodinvocationdialog.cpp
  modelpickerdialog.cpp
  palettemodel.cpp
  propertybinder.cpp
  propertywidget.cpp
  propertywidgettab.cpp
  proxytooluifactory.cpp
  visibilityfilterproxymodel.cpp
  splashscreen.cpp
  searchlinecontroller.cpp
  tooluifactory.cpp
  uiintegration.cpp
  uistatemanager.cpp
  uiresources.cpp
  remoteviewwidget.cpp
  trailingcolorlabel.cpp
  helpcontroller.cpp

  paintanalyzerwidget.cpp
  paintanalyzerreplayview.cpp
  paintbufferclientmodel.cpp
  paintbufferviewer.cpp

  propertyeditor/propertycoloreditor.cpp
  propertyeditor/propertydoublepaireditor.cpp
  propertyeditor/propertyeditorfactory.cpp
  propertyeditor/propertyextendededitor.cpp
  propertyeditor/propertyfonteditor.cpp
  propertyeditor/propertyintpaireditor.cpp
  propertyeditor/propertypaletteeditor.cpp
  propertyeditor/palettedialog.cpp
  propertyeditor/propertyeditordelegate.cpp
  propertyeditor/propertyenumeditor.cpp
  propertyeditor/propertymatrixdialog.cpp
  propertyeditor/propertymatrixeditor.cpp
  propertyeditor/propertymatrixmodel.cpp
  propertyeditor/propertytexteditor.cpp
  propertyeditor/propertyrecteditor.cpp

  codeeditor/codeeditor.cpp
  codeeditor/codeeditorsidebar.cpp

  tools/messagehandler/messagedisplaymodel.cpp
  tools/messagehandler/messagehandlerwidget.cpp
  tools/messagehandler/messagehandlerclient.cpp
  tools/metaobjectbrowser/metaobjectbrowserwidget.cpp
  tools/metaobjectbrowser/metaobjecttreeclientproxymodel.cpp
  tools/metatypebrowser/metatypebrowserwidget.cpp
  tools/metatypebrowser/metatypesclientmodel.cpp
  tools/metatypebrowser/metatypebrowserclient.cpp
  tools/objectinspector/objectinspectorwidget.cpp
  tools/objectinspector/propertiestab.cpp
  tools/objectinspector/propertiesextensionclient.cpp
  tools/objectinspector/clientmethodmodel.cpp
  tools/objectinspector/methodsextensionclient.cpp
  tools/objectinspector/methodstab.cpp
  tools/objectinspector/connectionstab.cpp
  tools/objectinspector/connectionsclientproxymodel.cpp
  tools/objectinspector/connectionsextensionclient.cpp
  tools/objectinspector/enumstab.cpp
  tools/objectinspector/classinfotab.cpp
  tools/objectinspector/methodstab.cpp
  tools/objectinspector/applicationattributetab.cpp
  tools/objectinspector/bindingtab.cpp
  tools/objectinspector/stacktracetab.cpp
  tools/problemreporter/problemreporterwidget.cpp
  tools/problemreporter/problemreporterclient.cpp
  tools/problemreporter/problemclientmodel.cpp
  tools/resourcebrowser/clientresourcemodel.cpp
  tools/resourcebrowser/resourcebrowserwidget.cpp
  tools/resourcebrowser/resourcebrowserclient.cpp

  resources/ui.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/resources/classicons.qrc
)

add_library(gammaray_ui SHARED ${gammaray_ui_srcs})
generate_export_header(gammaray_ui)
set_target_properties(gammaray_ui PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
)
gammaray_set_rpath(gammaray_ui ${LIB_INSTALL_DIR})

gammaray_target_relocatable_interfaces(gammaray_ui_ipaths)
target_include_directories(gammaray_ui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_ui_ipaths}>)
if(HAVE_PRIVATE_QT_HEADERS)
    target_include_directories(gammaray_ui SYSTEM PRIVATE ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
else(HAVE_PRIVATE_QT_HEADERS)
  add_feature_info("QHeaderView geometry/state restoration" HAVE_PRIVATE_QT_HEADERS
    "You must have a build version of Qt available. Make sure the qmake found first in your execute comes from this build version."
  )
endif(HAVE_PRIVATE_QT_HEADERS)

target_link_libraries(gammaray_ui
  LINK_PUBLIC ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} gammaray_common
  LINK_PRIVATE gammaray_common_internal gammaray_kitemmodels
)
if(TARGET KF5::SyntaxHighlighting)
  target_link_libraries(gammaray_ui LINK_PRIVATE KF5::SyntaxHighlighting)
endif()

set(gammaray_ui_internal_srcs
  sidepane.cpp
  aboutpluginsdialog.cpp
  mainwindow.cpp
  clienttoolfilterproxymodel.cpp
  aboutdialog.cpp
)

add_library(gammaray_ui_internal STATIC ${gammaray_ui_internal_srcs})
set_target_properties(gammaray_ui_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(gammaray_ui_internal SYSTEM PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
target_link_libraries(gammaray_ui_internal
  ${Qt5Core_LIBRARIES}
  ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES}
  gammaray_common_internal
  gammaray_ui
  gammaray_kitemmodels
)

if(NOT GAMMARAY_DISABLE_FEEDBACK)
    target_link_libraries(gammaray_ui_internal gammaray_kuserfeedback)
endif()

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_ui EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

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

  ecm_generate_pri_file(BASE_NAME GammaRayUi
                        LIB_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
                        DEPS "core gui widgets GammaRayCommon"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
  )

  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
else()
  install(TARGETS gammaray_ui ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_ui>/$<TARGET_PDB_FILE_NAME:gammaray_ui>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
add_subdirectory(resources)
