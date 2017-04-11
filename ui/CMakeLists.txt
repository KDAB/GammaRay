#
# Shared code between in-process and out-of-process UI
#
set(gammaray_ui_srcs
  aboutdata.cpp
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
  remoteviewwidget.cpp
  helpcontroller.cpp

  paintanalyzerwidget.cpp
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
  tools/resourcebrowser/clientresourcemodel.cpp
  tools/resourcebrowser/resourcebrowserwidget.cpp
  tools/resourcebrowser/resourcebrowserclient.cpp
  tools/standardpaths/standardpathswidget.cpp
)

qt4_wrap_ui(gammaray_ui_srcs
  methodinvocationdialog.ui

  paintanalyzerwidget.ui
  paintbufferviewer.ui

  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  propertyeditor/propertymatrixdialog.ui
  propertyeditor/propertytexteditor.ui
  propertyeditor/palettedialog.ui

  tools/messagehandler/messagehandlerwidget.ui
  tools/metatypebrowser/metatypebrowserwidget.ui
  tools/objectinspector/objectinspectorwidget.ui
  tools/objectinspector/propertiestab.ui
  tools/objectinspector/methodstab.ui
  tools/objectinspector/connectionstab.ui
  tools/objectinspector/enumstab.ui
  tools/objectinspector/classinfotab.ui
  tools/objectinspector/methodstab.ui
  tools/objectinspector/applicationattributetab.ui
  tools/resourcebrowser/resourcebrowserwidget.ui
  tools/standardpaths/standardpathswidget.ui
)

qt4_add_resources(gammaray_ui_srcs resources/ui.qrc)

add_library(gammaray_ui SHARED ${gammaray_ui_srcs})
generate_export_header(gammaray_ui)
set_target_properties(gammaray_ui PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
)

gammaray_target_relocatable_interfaces(gammaray_ui_ipaths)
target_include_directories(gammaray_ui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_ui_ipaths}>)
if(HAVE_PRIVATE_QT_HEADERS)
  if(Qt5Widgets_FOUND)
    include_directories(SYSTEM ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
  elseif(QtGui_FOUND)
    include_directories(SYSTEM ${QtGui_PRIVATE_INCLUDE_DIRS})
  endif()
else(HAVE_PRIVATE_QT_HEADERS)
  add_feature_info("QHeaderView geometry/state restoration" HAVE_PRIVATE_QT_HEADERS
    "You must have a build version of Qt available. Make sure the qmake found first in your execute comes from this build version."
  )
endif(HAVE_PRIVATE_QT_HEADERS)

target_link_libraries(gammaray_ui
  LINK_PUBLIC ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common
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

qt4_wrap_ui(gammaray_ui_internal_srcs
  mainwindow.ui
  aboutdialog.ui
)

add_library(gammaray_ui_internal STATIC ${gammaray_ui_internal_srcs})
set_target_properties(gammaray_ui_internal PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gammaray_ui_internal
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_common_internal
  gammaray_ui
  gammaray_kitemmodels
)

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_ui EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

  gammaray_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_ui_export.h
    clienttoolmanager.h
    propertyeditor/propertyeditordelegate.h
    propertywidget.h
    propertywidgettab.h
    tooluifactory.h
    uiintegration.h
    uistatemanager.h
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
