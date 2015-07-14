#
# Shared code between in-process and out-of-process UI
#
set(gammaray_ui_srcs
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/krecursivefilterproxymodel.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kfilterproxysearchline.cpp

  aboutdata.cpp
  clienttoolmodel.cpp
  deferredresizemodesetter.cpp
  deferredtreeviewconfiguration.cpp
  editabletypesmodel.cpp
  methodinvocationdialog.cpp
  palettemodel.cpp
  propertybinder.cpp
  propertywidget.cpp
  proxytooluifactory.cpp
  splashscreen.cpp

  propertyeditor/propertycoloreditor.cpp
  propertyeditor/propertydoublepaireditor.cpp
  propertyeditor/propertyeditorfactory.cpp
  propertyeditor/propertyextendededitor.cpp
  propertyeditor/propertyfonteditor.cpp
  propertyeditor/propertyintpaireditor.cpp
  propertyeditor/propertypaletteeditor.cpp
  propertyeditor/palettedialog.cpp
  propertyeditor/propertyeditordelegate.cpp

  tools/connectioninspector/connectioninspectorwidget.cpp
  tools/localeinspector/localeinspectorwidget.cpp
  tools/messagehandler/messagehandlerwidget.cpp
  tools/messagehandler/messagehandlerclient.cpp
  ../core/tools/messagehandler/messagehandlerinterface.cpp
  tools/metaobjectbrowser/metaobjectbrowserwidget.cpp
  tools/metatypebrowser/metatypebrowserwidget.cpp
  tools/mimetypes/mimetypeswidget.cpp
  tools/modelinspector/modelinspectorwidget.cpp
  tools/modelinspector/modelinspectorclient.cpp
  tools/objectinspector/objectinspectorwidget.cpp
  tools/objectinspector/propertiestab.cpp
  tools/objectinspector/propertiesextensionclient.cpp
  tools/objectinspector/methodsextensionclient.cpp
  tools/objectinspector/methodstab.cpp
  tools/objectinspector/connectionstab.cpp
  tools/objectinspector/connectionsclientproxymodel.cpp
  tools/objectinspector/connectionsextensionclient.cpp
  tools/objectinspector/enumstab.cpp
  tools/objectinspector/classinfotab.cpp
  tools/objectinspector/methodstab.cpp
  tools/resourcebrowser/clientresourcemodel.cpp
  tools/resourcebrowser/resourcebrowserwidget.cpp
  tools/resourcebrowser/resourcebrowserclient.cpp
  tools/standardpaths/standardpathswidget.cpp
  tools/textdocumentinspector/textdocumentinspectorwidget.cpp
  tools/textdocumentinspector/textdocumentcontentview.cpp
)

qt4_wrap_ui(gammaray_ui_srcs
  methodinvocationdialog.ui

  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  propertyeditor/palettedialog.ui

  tools/connectioninspector/connectioninspectorwidget.ui
  tools/localeinspector/localeinspectorwidget.ui
  tools/messagehandler/messagehandlerwidget.ui
  tools/metatypebrowser/metatypebrowserwidget.ui
  tools/mimetypes/mimetypeswidget.ui
  tools/modelinspector/modelinspectorwidget.ui
  tools/objectinspector/objectinspectorwidget.ui
  tools/objectinspector/propertiestab.ui
  tools/objectinspector/methodstab.ui
  tools/objectinspector/connectionstab.ui
  tools/objectinspector/enumstab.ui
  tools/objectinspector/classinfotab.ui
  tools/objectinspector/methodstab.ui
  tools/resourcebrowser/resourcebrowserwidget.ui
  tools/standardpaths/standardpathswidget.ui
  tools/textdocumentinspector/textdocumentinspectorwidget.ui
)

add_library(gammaray_ui SHARED ${gammaray_ui_srcs})
set_target_properties(gammaray_ui PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  DEFINE_SYMBOL MAKE_GAMMARAY_UI_LIB
  OUTPUT_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
)

target_link_libraries(gammaray_ui
  LINK_PUBLIC ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common
  LINK_PRIVATE gammaray_common_internal
)

install(TARGETS gammaray_ui EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

set(gammaray_ui_internal_srcs
  sidepane.cpp
  aboutpluginsdialog.cpp
  mainwindow.cpp
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
)

gammaray_install_headers(
  gammaray_ui_export.h
  clienttoolmodel.h
  deferredresizemodesetter.h
  deferredtreeviewconfiguration.h
  propertywidget.h
  propertywidgettab.h
  tooluifactory.h
)

ecm_generate_pri_file(BASE_NAME GammaRayUi
                      LIB_NAME gammaray_ui-${GAMMARAY_PROBE_ABI}
                      DEPS "core gui widgets GammaRayCommon"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
