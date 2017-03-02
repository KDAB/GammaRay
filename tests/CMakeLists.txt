# keep test output locally, in the multibuild case
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

add_subdirectory(shared)
configure_file(gammaray-test-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/gammaray-test-config.h)

add_library(modeltestobj OBJECT ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.cpp)
set_target_properties(modeltestobj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(modeltestobj PRIVATE ${Qt5Core_DEFINITIONS}) # for -DQT_NAMESPACE=...
target_include_directories(modeltestobj PUBLIC ${QT_INCLUDES})

### BENCH SUITE

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_executable(benchsuite benchsuite.cpp)

  target_link_libraries(benchsuite
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    gammaray_common
    gammaray_core
  )

### CONNECTIONTEST

  add_executable(connectiontest test_connections.cpp)

  target_link_libraries(connectiontest
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
  )

endif()

add_executable(attachhelper attachhelper.cpp)

target_link_libraries(attachhelper ${QT_QTCORE_LIBRARIES})

set(RUN_FILTER_TESTS OFF
  CACHE BOOL "Run filter tests which fail due to a bug in QSortFilterProxyModel"
)

if(UNIX)
  find_program(GDB_EXECUTABLE gdb)
  find_program(LLDB_EXECUTABLE lldb)
endif()

set(INJECTOR_TYPES)
if(UNIX)
  # OSX+ASAN+Qt4 (triggers a bug in Qt4, see http://stackoverflow.com/questions/22696071/how-to-make-addresssanitizer-not-stop-after-one-error-and-other-issues)
  string(REGEX MATCH -fsanitize=address ASAN_ENABLED ${CMAKE_CXX_FLAGS})
  if(APPLE AND ASAN_ENABLED AND NOT Qt5Core_FOUND)
    set(OSX_ASAN_WORKAROUND ON)
  endif()

  if(NOT OSX_ASAN_WORKAROUND)
    list(APPEND INJECTOR_TYPES preload style)
  endif()

  if(GDB_EXECUTABLE)
    list(APPEND INJECTOR_TYPES gdb)
  endif()
  if(LLDB_EXECUTABLE AND NOT OSX_ASAN_WORKAROUND)
    list(APPEND INJECTOR_TYPES lldb)
  endif()
elseif(WIN32)
  list(APPEND INJECTOR_TYPES windll style)
endif()

foreach(injectorType ${INJECTOR_TYPES})
  add_test(connectiontest-${injectorType}
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess --listen tcp://127.0.0.1 -unittest -i ${injectorType}
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
  )
  if(RUN_FILTER_TESTS)
    add_test(connectiontest-${injectorType}-filter
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray -unittest --listen tcp://127.0.0.1 -filtertest -i ${injectorType}
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
  endif()
endforeach()

if(NOT OSX_ASAN_WORKAROUND)
add_test(NAME attachtest-gdb
  COMMAND ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/attachhelper ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray gdb
  ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
)
add_test(NAME attachtest-lldb
  COMMAND ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/attachhelper ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray lldb
  ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
)
add_test(NAME attachtest-windll
  COMMAND ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/attachhelper ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray windll
  ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
)
endif()

### MultiSignalMapper test

add_executable(multisignalmappertest multisignalmappertest.cpp ../core/multisignalmapper.cpp)
target_link_libraries(multisignalmappertest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTTEST_LIBRARIES}
)
add_test(multisignalmappertest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multisignalmappertest)

### source location test

add_executable(sourcelocationtest sourcelocationtest.cpp)
target_link_libraries(sourcelocationtest ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common)
add_test(NAME sourcelocationtest COMMAND sourcelocationtest)

### self locator test

add_executable(selflocatortest selflocatortest.cpp)
target_link_libraries(selflocatortest ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_common)
if(NOT WIN32 AND NOT QNXNTO AND NOT ANDROID)
  target_link_libraries(selflocatortest dl)
endif()
add_test(NAME selflocatortest COMMAND selflocatortest)

### Probe ABI test

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
add_executable(probeabitest probeabitest.cpp)
target_link_libraries(probeabitest gammaray_launcher ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(probeabitest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/probeabitest)

add_executable(probeabidetectortest probeabidetectortest.cpp)
target_link_libraries(probeabidetectortest gammaray_launcher ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(probeabidetectortest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/probeabidetectortest)

### self test test
if (NOT OSX_ASAN_WORKAROUND)
add_executable(selftesttest selftesttest.cpp)
target_link_libraries(selftesttest gammaray_launcher gammaray_common ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES})
add_test(NAME selftesttest COMMAND selftesttest)
endif()

### probe settings test
add_executable(probesettingsclient probesettingsclient.cpp ${CMAKE_SOURCE_DIR}/core/probesettings.cpp)
target_compile_definitions(probesettingsclient PRIVATE GAMMARAY_CORE_STATIC_DEFINE)
target_include_directories(probesettingsclient PRIVATE ${CMAKE_BINARY_DIR}/core)
target_link_libraries(probesettingsclient gammaray_common ${QT_QTNETWORK_LIBRARIES})
add_executable(probesettingstest probesettingstest.cpp)
target_link_libraries(probesettingstest gammaray_launcher gammaray_common ${QT_QTCORE_LIBRARIES} ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME probesettingstest COMMAND probesettingstest)

endif()

### Remote model tests

if(GAMMARAY_BUILD_UI AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  add_executable(remotemodeltest
    remotemodeltest.cpp
    $<TARGET_OBJECTS:modeltestobj>
    ../core/remote/remotemodelserver.cpp
  )
  target_link_libraries(remotemodeltest gammaray_core gammaray_client ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
  add_test(NAME remotemodeltest COMMAND remotemodeltest)

  add_executable(networkselectionmodeltest
    networkselectionmodeltest.cpp
    ${CMAKE_SOURCE_DIR}/common/networkselectionmodel.cpp
  )
  target_link_libraries(networkselectionmodeltest gammaray_common ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
  add_test(NAME networkselectionmodeltest COMMAND networkselectionmodeltest)
endif()

### QSignalSpyCallback tests

if(NOT OSX_ASAN_WORKAROUND)
add_executable(signalspycallbacktest
  signalspycallbacktest.cpp
  ../probe/probecreator.cpp
)
target_link_libraries(signalspycallbacktest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME signalspycallbacktest COMMAND signalspycallbacktest)
endif()

### multi-threading test

if(Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
add_executable(multithreadingtest
  multithreadingtest.cpp
  ../probe/probecreator.cpp
  ../probe/hooks.cpp
)
target_link_libraries(multithreadingtest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME multithreadingtest COMMAND multithreadingtest)

### QTranslator test
#does not work unless the translations are installed in QT_INSTALL_TRANSLATIONS
if(EXISTS "${QT_INSTALL_TRANSLATIONS}/qtbase_de.qm")
  add_executable(translatortest
    translatortest.cpp
    ../probe/probecreator.cpp
    ../probe/hooks.cpp
    $<TARGET_OBJECTS:modeltestobj>
  )
  target_link_libraries(translatortest gammaray_core ${QT_QTTEST_LIBRARIES})
  add_test(NAME translatortest COMMAND translatortest)
else()
  message(STATUS "WARNING: Skipping the translatortest since the translations are not installed.")
endif()

endif()

### Integration test

if(NOT OSX_ASAN_WORKAROUND)
  add_executable(integrationtest
    integrationtest.cpp
      $<TARGET_OBJECTS:gammaray_probe_obj>
      $<TARGET_OBJECTS:modeltestobj>
  )
  target_link_libraries(integrationtest $<TARGET_PROPERTY:gammaray_probe,LINK_LIBRARIES> gammaray_core ${QT_QTTEST_LIBRARIES})
  add_test(NAME integrationtest COMMAND integrationtest)
endif()

### Model inspector test

if(Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4 AND GAMMARAY_BUILD_UI) # requires QHooks
  add_executable(modelinspectortest
    modelinspectortest.cpp
    ${CMAKE_SOURCE_DIR}/plugins/modelinspector/modelinspectorinterface.cpp
    $<TARGET_OBJECTS:gammaray_probe_obj>
    $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(modelinspectortest gammaray_core gammaray_ui ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME modelinspectortest COMMAND modelinspectortest)
endif()

### QWidget test

if(Qt5Widgets_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
add_executable(widgettest
  widgettest.cpp
  $<TARGET_OBJECTS:gammaray_probe_obj>
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(widgettest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTWIDGETS_LIBRARIES})

add_test(NAME widgettest COMMAND widgettest)
endif()

### QAction test

if(Qt5Widgets_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
add_executable(actiontest
  actiontest.cpp
  ${CMAKE_SOURCE_DIR}/plugins/actioninspector/clientactionmodel.cpp
  $<TARGET_OBJECTS:gammaray_probe_obj>
)
target_link_libraries(actiontest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTWIDGETS_LIBRARIES})

add_test(NAME actiontest COMMAND actiontest)
endif()

### MetaObject test

add_executable(metaobjecttest metaobjecttest.cpp)
target_link_libraries(metaobjecttest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME metaobjecttest COMMAND metaobjecttest)

### ObjectInstance test

add_executable(objectinstancetest objectinstancetest.cpp)
target_link_libraries(objectinstancetest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME objectinstancetest COMMAND objectinstancetest)

### PropertySyncer test

add_executable(propertysyncertest propertysyncertest.cpp)
target_link_libraries(propertysyncertest gammaray_common ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES})
add_test(NAME propertysyncertest COMMAND propertysyncertest)

### PropertyBinder test

if(GAMMARAY_BUILD_UI)
add_executable(propertybindertest propertybindertest.cpp)
target_link_libraries(propertybindertest gammaray_ui ${QT_QTTEST_LIBRARIES})
add_test(NAME propertybindertest COMMAND propertybindertest)
endif()

### Property Adaptor test

add_executable(propertyadaptortest propertyadaptortest.cpp)
target_link_libraries(propertyadaptortest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES} gammaray_shared_test_data)
add_test(NAME propertyadaptortest COMMAND propertyadaptortest)

### Enum property test

add_executable(enumpropertytest enumpropertytest.cpp)
target_link_libraries(enumpropertytest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME enumpropertytest COMMAND enumpropertytest)

### Property model test

add_executable(propertymodeltest
  propertymodeltest.cpp
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(propertymodeltest gammaray_core ${QT_QTTEST_LIBRARIES} gammaray_shared_test_data)
add_test(NAME propertymodeltest COMMAND propertymodeltest)

### Method model test

add_executable(methodmodeltest
  methodmodeltest.cpp
  ${CMAKE_SOURCE_DIR}/core/objectmethodmodel.cpp
  ${CMAKE_SOURCE_DIR}/core/qmetaobjectvalidator.cpp
  ${CMAKE_SOURCE_DIR}/ui/tools/objectinspector/clientmethodmodel.cpp
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(methodmodeltest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME methodmodeltest COMMAND methodmodeltest)

### QMetaObject validator test

add_executable(qmetaobjectvalidatortest
  qmetaobjectvalidatortest.cpp
)
target_link_libraries(qmetaobjectvalidatortest ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME qmetavobjectvalidatortest COMMAND qmetaobjectvalidatortest)

### Meta object tree model test

if(Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
  add_executable(metaobjecttreemodeltest
    metaobjecttreemodeltest.cpp
    ${CMAKE_SOURCE_DIR}/ui/tools/metaobjectbrowser/metaobjecttreeclientproxymodel.cpp
    $<TARGET_OBJECTS:gammaray_probe_obj>
    $<TARGET_OBJECTS:modeltestobj>
  )
  target_link_libraries(metaobjecttreemodeltest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
  add_test(NAME metaobjecttreemodeltest COMMAND metaobjecttreemodeltest)
endif()

### Meta type browser

add_executable(metatypemodeltest
  metatypemodeltest.cpp
  ${CMAKE_SOURCE_DIR}/core/tools/metatypebrowser/metatypesmodel.cpp
  ${CMAKE_SOURCE_DIR}/ui/tools/metatypebrowser/metatypesclientmodel.cpp
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(metatypemodeltest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
add_test(NAME metatypemodeltest COMMAND metatypemodeltest)

### Font plugin

add_executable(fontdatabasemodeltest
  fontdatabasemodeltest.cpp
  ${CMAKE_SOURCE_DIR}/plugins/fontbrowser/fontdatabasemodel.cpp
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(fontdatabasemodeltest ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES})
add_test(NAME fontdatabasemodeltest COMMAND fontdatabasemodeltest)

### Codec plugins

if(NOT OSX_ASAN_WORKAROUND)
add_executable(codecmodeltest
  codecmodeltest.cpp
  ${CMAKE_SOURCE_DIR}/plugins/codecbrowser/codecmodel.cpp
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(codecmodeltest ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES})
add_test(NAME codecmodeltest COMMAND codecmodeltest)
endif()

### Timertop plugin

if(Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
  add_executable(timertoptest
    timertoptest.cpp
    $<TARGET_OBJECTS:modeltestobj>
    ${CMAKE_SOURCE_DIR}/probe/probecreator.cpp
    ${CMAKE_SOURCE_DIR}/probe/hooks.cpp
  )
  target_link_libraries(timertoptest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})
  add_test(NAME timertoptest COMMAND timertoptest)
endif()

### QML support

if(Qt5Quick_FOUND)
  add_executable(qmlsupporttest
    qmlsupporttest.cpp
    ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmllistpropertyadaptor.cpp
    ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmlattachedpropertyadaptor.cpp
    ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qjsvaluepropertyadaptor.cpp
    ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmlcontextpropertyadaptor.cpp
  )
  include_directories(SYSTEM ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
  target_link_libraries(qmlsupporttest gammaray_core Qt5::Quick Qt5::Test)
  add_test(NAME qmlsupporttest COMMAND qmlsupporttest)
endif()

### Quick inspector

if(Qt5Quick_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
set(quickinspectortest_srcs
  quickinspectortest.cpp
  ../plugins/quickinspector/quickinspectorinterface.cpp
  ../plugins/quickinspector/quickitemgeometry.cpp
  $<TARGET_OBJECTS:modeltestobj>
  $<TARGET_OBJECTS:gammaray_probe_obj>
)
qt4_add_resources(quickinspectortest_srcs quickinspectortest.qrc)
add_executable(quickinspectortest ${quickinspectortest_srcs})
target_include_directories(quickinspectortest SYSTEM PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
target_link_libraries(quickinspectortest gammaray_core Qt5::Test Qt5::Quick)
add_test(NAME quickinspectortest COMMAND quickinspectortest)
endif()

### ToolManager test

if(GAMMARAY_BUILD_UI AND Qt5Widgets_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
add_executable(toolmanagertest
  toolmanagertest.cpp
  ../common/toolmanagerinterface.cpp
  $<TARGET_OBJECTS:gammaray_probe_obj>
  $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(toolmanagertest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTWIDGETS_LIBRARIES})
target_link_libraries(toolmanagertest gammaray_ui)
add_test(NAME toolmanagertest COMMAND toolmanagertest)
endif()

if(NOT GAMMARAY_PROBE_ONLY_BUILD AND NOT OSX_ASAN_WORKAROUND)
    add_executable(launchertest launchertest.cpp)
    target_include_directories(launchertest PRIVATE ${CMAKE_BINARY_DIR}/launcher)
    target_link_libraries(launchertest gammaray_core gammaray_launcher ${QT_QTTEST_LIBRARIES})
    add_test(NAME launchertest COMMAND launchertest)
endif()

add_subdirectory(manual)
add_subdirectory(targets)
