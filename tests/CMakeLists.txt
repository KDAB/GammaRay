# keep test output locally, in the multibuild case
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

configure_file(gammaray-test-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/gammaray-test-config.h)

# create intermediate modeltestobj object files which unit tests can use
add_library(modeltestobj OBJECT ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.cpp)
set_target_properties(modeltestobj PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(Qt5Core_FOUND)
    target_compile_definitions(modeltestobj PRIVATE ${Qt5Core_DEFINITIONS}) # for -DQT_NAMESPACE=...
    target_include_directories(modeltestobj PUBLIC ${Qt5Core_INCLUDE_DIRS})
else()
    target_include_directories(modeltestobj PUBLIC ${QT_QTCORE_INCLUDE_DIRS})
endif()

# create intermediate test_helpers_obj object files which unit tests can use
add_library(test_helpers_obj OBJECT testhelpers.cpp)
set_target_properties(test_helpers_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(Qt5Core_FOUND)
    target_compile_definitions(test_helpers_obj PRIVATE ${Qt5Core_DEFINITIONS} ${Qt5Test_DEFINITIONS}) # for -DQT_NAMESPACE=...
    target_include_directories(test_helpers_obj PUBLIC
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Test_INCLUDE_DIRS}
    )
else()
    target_include_directories(test_helpers_obj PUBLIC
        ${QT_QTCORE_INCLUDE_DIRS}
        ${QT_QTTEST_INCLUDE_DIRS}
    )
endif()

if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
# create intermediate base_probe_test_obj object files which unit tests can use
add_library(base_probe_test_obj OBJECT baseprobetest.h)
set_target_properties(base_probe_test_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(base_probe_test_obj PUBLIC
    $<TARGET_PROPERTY:gammaray_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:gammaray_common,INTERFACE_INCLUDE_DIRECTORIES>
)
if(Qt5Core_FOUND)
    target_compile_definitions(base_probe_test_obj PRIVATE ${Qt5Core_DEFINITIONS} ${Qt5Test_DEFINITIONS}) # for -DQT_NAMESPACE=...
    target_include_directories(base_probe_test_obj PUBLIC
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Test_INCLUDE_DIRS}
    )
else()
    target_include_directories(base_probe_test_obj PUBLIC
        ${QT_QTCORE_INCLUDE_DIRS}
        ${QT_QTTEST_INCLUDE_DIRS}
    )
endif()

if(Qt5Quick_FOUND)
    # create intermediate base_quick_test_obj object files which unit tests can use
    add_library(base_quick_test_obj OBJECT basequicktest.h)
    set_target_properties(base_quick_test_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(base_quick_test_obj PRIVATE ${Qt5Core_DEFINITIONS} ${Qt5Test_DEFINITIONS} ${Qt5Quick_DEFINITIONS}) # for -DQT_NAMESPACE=...
    target_include_directories(base_quick_test_obj PUBLIC
        $<TARGET_PROPERTY:base_probe_test_obj,INTERFACE_INCLUDE_DIRECTORIES>
    )
    target_include_directories(base_quick_test_obj PUBLIC
        ${Qt5Quick_INCLUDE_DIRS}
    )
endif()
endif()

add_subdirectory(shared)
add_subdirectory(manual)
add_subdirectory(targets)

# plain unit tests
function(gammaray_add_test _name)
    add_executable(${_name} ${ARGN}
        $<TARGET_OBJECTS:test_helpers_obj>
    )
    gammaray_set_rpath(${_name} ${BIN_INSTALL_DIR})
    target_link_libraries(${_name} ${QT_QTTEST_LIBRARIES})
    add_test(NAME ${_name} COMMAND ${_name})
endfunction()

# unit tests requiring probe injection
function(gammaray_add_probe_test _name)
    add_executable(${_name} ${ARGN}
        $<TARGET_OBJECTS:gammaray_probe_obj>
        $<TARGET_OBJECTS:test_helpers_obj>
        $<TARGET_OBJECTS:base_probe_test_obj>
    )
    gammaray_set_rpath(${_name} ${BIN_INSTALL_DIR})
    target_link_libraries(${_name} $<TARGET_PROPERTY:gammaray_probe,LINK_LIBRARIES> ${QT_QTTEST_LIBRARIES})
    add_test(NAME ${_name} COMMAND ${_name})
endfunction()

# unit tests requiring quick and probe injection
function(gammaray_add_quick_test _name)
    add_executable(${_name} ${ARGN}
        $<TARGET_OBJECTS:gammaray_probe_obj>
        $<TARGET_OBJECTS:test_helpers_obj>
        $<TARGET_OBJECTS:base_probe_test_obj>
        $<TARGET_OBJECTS:base_quick_test_obj>
    )
    gammaray_set_rpath(${_name} ${BIN_INSTALL_DIR})
    target_link_libraries(${_name} $<TARGET_PROPERTY:gammaray_probe,LINK_LIBRARIES> ${QT_QTTEST_LIBRARIES})
    add_test(NAME ${_name} COMMAND ${_name})
endfunction()

### BENCH SUITE

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_executable(benchsuite benchsuite.cpp)
  gammaray_set_rpath(benchsuite ${BIN_INSTALL_DIR})

  target_link_libraries(benchsuite
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    gammaray_common
    gammaray_core
  )

#
# Full-scale injection tests
#
  add_executable(connectiontest test_connections.cpp)
  gammaray_set_rpath(connectiontest ${BIN_INSTALL_DIR})

  target_link_libraries(connectiontest
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
  )

endif()

add_executable(attachhelper attachhelper.cpp)
gammaray_set_rpath(attachhelper ${BIN_INSTALL_DIR})

target_link_libraries(attachhelper ${QT_QTCORE_LIBRARIES})

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
  add_test(connectiontest-${injectorType}-filter
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess -unittest --listen tcp://127.0.0.1 -filtertest -i ${injectorType}
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
  )
  set_tests_properties(connectiontest-${injectorType}-filter PROPERTIES ENVIRONMENT GAMMARAY_TEST_FILTER=1)
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

#
# common and core unit tests
#
gammaray_add_test(multisignalmappertest multisignalmappertest.cpp ../core/multisignalmapper.cpp)
target_link_libraries(multisignalmappertest ${QT_QTGUI_LIBRARIES})

gammaray_add_test(sourcelocationtest sourcelocationtest.cpp)
target_link_libraries(sourcelocationtest ${QT_QTGUI_LIBRARIES} gammaray_common)

gammaray_add_test(selflocatortest selflocatortest.cpp)
target_link_libraries(selflocatortest ${QT_QTGUI_LIBRARIES} gammaray_common ${CMAKE_DL_LIBS})

gammaray_add_test(executiontest executiontest.cpp)
target_link_libraries(executiontest ${QT_QTGUI_LIBRARIES} gammaray_core)

gammaray_add_test(metaobjecttest metaobjecttest.cpp)
target_link_libraries(metaobjecttest gammaray_core)

gammaray_add_test(objectinstancetest objectinstancetest.cpp)
target_link_libraries(objectinstancetest gammaray_core)

gammaray_add_test(propertysyncertest propertysyncertest.cpp)
target_link_libraries(propertysyncertest gammaray_common ${QT_QTGUI_LIBRARIES})

gammaray_add_test(propertyadaptortest propertyadaptortest.cpp)
target_link_libraries(propertyadaptortest gammaray_core ${QT_QTGUI_LIBRARIES} gammaray_shared_test_data)

if(HAVE_QT_WIDGETS)
    gammaray_add_test(enumpropertytest enumpropertytest.cpp)
    target_link_libraries(enumpropertytest gammaray_core ${QT_QTGUI_LIBRARIES})
endif()

gammaray_add_test(propertymodeltest propertymodeltest.cpp $<TARGET_OBJECTS:modeltestobj>)
target_link_libraries(propertymodeltest gammaray_core gammaray_shared_test_data)

gammaray_add_test(qmetaobjectvalidatortest qmetaobjectvalidatortest.cpp)
target_include_directories(qmetaobjectvalidatortest SYSTEM PRIVATE ${Qt5Core_PRIVATE_INCLUDE_DIRS})
target_link_libraries(qmetaobjectvalidatortest ${QT_QTGUI_LIBRARIES} gammaray_core)

if(GAMMARAY_BUILD_UI)
    gammaray_add_test(metatypemodeltest
        metatypemodeltest.cpp
        ${CMAKE_SOURCE_DIR}/core/tools/metatypebrowser/metatypesmodel.cpp
        ${CMAKE_SOURCE_DIR}/ui/tools/metatypebrowser/metatypesclientmodel.cpp
        $<TARGET_OBJECTS:modeltestobj>
    )
    target_link_libraries(metatypemodeltest gammaray_core ${QT_QTGUI_LIBRARIES})
endif()

if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND NOT OSX_ASAN_WORKAROUND)
    gammaray_add_probe_test(signalspycallbacktest signalspycallbacktest.cpp)
    gammaray_add_probe_test(integrationtest integrationtest.cpp)
endif()

if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
    gammaray_add_probe_test(multithreadingtest multithreadingtest.cpp)

    if(GAMMARAY_BUILD_UI)
        gammaray_add_probe_test(methodmodeltest
            methodmodeltest.cpp
            ${CMAKE_SOURCE_DIR}/core/objectmethodmodel.cpp
            ${CMAKE_SOURCE_DIR}/core/qmetaobjectvalidator.cpp
            ${CMAKE_SOURCE_DIR}/core/metaobjectregistry.cpp
            ${CMAKE_SOURCE_DIR}/ui/tools/objectinspector/clientmethodmodel.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_include_directories(methodmodeltest SYSTEM PRIVATE ${Qt5Core_PRIVATE_INCLUDE_DIRS})
        target_link_libraries(methodmodeltest ${QT_QTGUI_LIBRARIES})
    endif()
endif()


#
# ui unit tests
#
if(GAMMARAY_BUILD_UI)
    gammaray_add_test(propertybindertest propertybindertest.cpp)
    target_link_libraries(propertybindertest gammaray_ui)

    if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
        gammaray_add_probe_test(metaobjecttreemodeltest
            metaobjecttreemodeltest.cpp
            ${CMAKE_SOURCE_DIR}/ui/tools/metaobjectbrowser/metaobjecttreeclientproxymodel.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_link_libraries(metaobjecttreemodeltest gammaray_ui Qt5::Gui)

        gammaray_add_probe_test(toolmanagertest
            toolmanagertest.cpp
            ../common/toolmanagerinterface.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_link_libraries(toolmanagertest gammaray_core gammaray_ui Qt5::Network)
    endif()
endif()


#
# launcher unit tests
#
add_executable(sleep sleep.cpp)
target_link_libraries(sleep ${QT_QTCORE_LIBRARIES})

if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND NOT GAMMARAY_PROBE_ONLY_BUILD)
    gammaray_add_test(probeabitest probeabitest.cpp)
    target_link_libraries(probeabitest gammaray_launcher ${QT_QTGUI_LIBRARIES})

    gammaray_add_test(probeabidetectortest probeabidetectortest.cpp)
    target_link_libraries(probeabidetectortest gammaray_launcher ${QT_QTGUI_LIBRARIES})

    if (NOT OSX_ASAN_WORKAROUND)
        gammaray_add_test(selftesttest selftesttest.cpp)
        target_link_libraries(selftesttest gammaray_launcher gammaray_common ${QT_QTGUI_LIBRARIES})

        gammaray_add_test(earlyexittest earlyexittest.cpp)
        target_include_directories(earlyexittest PRIVATE ${CMAKE_BINARY_DIR}/launcher)
        target_link_libraries(earlyexittest gammaray_core gammaray_launcher)

        if(HAVE_QT_WIDGETS)
            gammaray_add_test(launchertest launchertest.cpp)
            target_include_directories(launchertest PRIVATE ${CMAKE_BINARY_DIR}/launcher)
            target_link_libraries(launchertest gammaray_core gammaray_launcher)

        endif()

        gammaray_add_test(clientconnectiontest clientconnectiontest.cpp)
        target_link_libraries(clientconnectiontest gammaray_core gammaray_launcher gammaray_client)
    endif()
endif()


#
# client unit tests
#

if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_executable(probesettingsclient probesettingsclient.cpp ${CMAKE_SOURCE_DIR}/core/probesettings.cpp)
    target_compile_definitions(probesettingsclient PRIVATE GAMMARAY_CORE_STATIC_DEFINE)
    target_include_directories(probesettingsclient PRIVATE ${CMAKE_BINARY_DIR}/core)
    target_link_libraries(probesettingsclient gammaray_common ${QT_QTNETWORK_LIBRARIES})
    gammaray_set_rpath(probesettingsclient ${BIN_INSTALL_DIR})
    gammaray_add_test(probesettingstest probesettingstest.cpp)
    target_link_libraries(probesettingstest gammaray_launcher gammaray_common ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES})

    if(GAMMARAY_BUILD_UI)
        gammaray_add_test(launcheruiiptest launcheruiiptest.cpp)
        target_link_libraries(launcheruiiptest gammaray_launcher_ui gammaray_common ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

        gammaray_add_test(remotemodeltest
            remotemodeltest.cpp
            $<TARGET_OBJECTS:modeltestobj>
            ../core/remote/remotemodelserver.cpp
        )
        target_link_libraries(remotemodeltest gammaray_core gammaray_client ${QT_QTGUI_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})

        gammaray_add_test(networkselectionmodeltest
            networkselectionmodeltest.cpp
            ${CMAKE_SOURCE_DIR}/common/networkselectionmodel.cpp
        )
        target_link_libraries(networkselectionmodeltest gammaray_common ${QT_QTGUI_LIBRARIES})
    endif()
endif()


#
# plugin unit tests
#
gammaray_add_test(fontdatabasemodeltest
    fontdatabasemodeltest.cpp
    ${CMAKE_SOURCE_DIR}/plugins/fontbrowser/fontdatabasemodel.cpp
    $<TARGET_OBJECTS:modeltestobj>
)
target_link_libraries(fontdatabasemodeltest ${QT_QTGUI_LIBRARIES})

if(NOT OSX_ASAN_WORKAROUND)
    gammaray_add_test(codecmodeltest
        codecmodeltest.cpp
        ${CMAKE_SOURCE_DIR}/plugins/codecbrowser/codecmodel.cpp
        $<TARGET_OBJECTS:modeltestobj>
    )
    target_link_libraries(codecmodeltest ${QT_QTGUI_LIBRARIES})
endif()

if(NOT GAMMARAY_CLIENT_ONLY_BUILD AND Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
    #does not work unless the translations are installed in QT_INSTALL_TRANSLATIONS
    if(EXISTS "${QT_INSTALL_TRANSLATIONS}/qtbase_de.qm")
        gammaray_add_probe_test(translatortest
            translatortest.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
    else()
        message(STATUS "WARNING: Skipping the translatortest since the translations are not installed.")
    endif()

    gammaray_add_probe_test(timertoptest
        timertoptest.cpp
        $<TARGET_OBJECTS:modeltestobj>
    )
    target_link_libraries(timertoptest Qt5::Gui)

    if(Qt5Widgets_FOUND)
        gammaray_add_probe_test(widgettest
            widgettest.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_link_libraries(widgettest Qt5::Widgets)

        gammaray_add_probe_test(actiontest
            actiontest.cpp
            ${CMAKE_SOURCE_DIR}/plugins/actioninspector/clientactionmodel.cpp
        )
        target_link_libraries(actiontest Qt5::Widgets)
    endif()

    if(GAMMARAY_BUILD_UI)
        gammaray_add_probe_test(modelinspectortest
            modelinspectortest.cpp
            ${CMAKE_SOURCE_DIR}/plugins/modelinspector/modelinspectorinterface.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_link_libraries(modelinspectortest gammaray_ui Qt5::Gui)
    endif()

    if(Qt5Quick_FOUND)
        gammaray_add_test(qmlsupporttest
            qmlsupporttest.cpp
            ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmllistpropertyadaptor.cpp
            ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmlattachedpropertyadaptor.cpp
            ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qjsvaluepropertyadaptor.cpp
            ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmlcontextpropertyadaptor.cpp
        )
        target_include_directories(qmlsupporttest SYSTEM PRIVATE ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
        target_link_libraries(qmlsupporttest gammaray_core Qt5::Quick)
    endif()

    if(Qt5Quick_FOUND AND NOT Qt5Quick_VERSION VERSION_LESS 5.7) # requires MSVC 2013 or higher
        gammaray_add_quick_test(bindinginspectortest
            bindinginspectortest.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_include_directories(bindinginspectortest SYSTEM PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
        target_link_libraries(bindinginspectortest gammaray_core Qt5::Quick)
        target_sources(bindinginspectortest PUBLIC
            ${CMAKE_SOURCE_DIR}/plugins/quickinspector/quickimplicitbindingdependencyprovider.cpp)
        if(NOT Qt5Qml_VERSION VERSION_LESS 5.10)
            target_sources(bindinginspectortest PUBLIC
                ${CMAKE_SOURCE_DIR}/plugins/qmlsupport/qmlbindingprovider.cpp)
        endif()
    endif()

    if(Qt5Quick_FOUND)
        gammaray_add_quick_test(quickinspectortest
            quickinspectortest.cpp
            quickinspectortest.qrc
            $<TARGET_OBJECTS:modeltestobj>
        )
        add_test(NAME quickinspectortest_softwarecontext COMMAND quickinspectortest)
        set_tests_properties(quickinspectortest_softwarecontext PROPERTIES ENVIRONMENT "QT_QUICK_BACKEND=softwarecontext")
        target_link_libraries(quickinspectortest gammaray_core gammaray_quickinspector_shared Qt5::Quick)

        gammaray_add_quick_test(quickinspectortest2
            quickinspectortest2.cpp
            quickinspectortest.qrc
            $<TARGET_OBJECTS:modeltestobj>
            )
        add_test(NAME quickinspectortest2_softwarecontext COMMAND quickinspectortest2)
        set_tests_properties(quickinspectortest2_softwarecontext PROPERTIES ENVIRONMENT "QT_QUICK_BACKEND=softwarecontext")
        target_link_libraries(quickinspectortest2 gammaray_core gammaray_quickinspector_shared Qt5::Quick)

        gammaray_add_quick_test(quickinspectorpickingtest
            quickinspectorpickingtest.cpp
            quickinspectortest.qrc
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_link_libraries(quickinspectorpickingtest gammaray_core Qt5::Test Qt5::Quick)

        gammaray_add_quick_test(quickinspectorbench
            quickinspectorbench.cpp
            ../plugins/quickinspector/quickitemmodel.cpp
        )
        target_link_libraries(quickinspectorbench gammaray_core Qt5::Test Qt5::Quick)

        gammaray_add_quick_test(quicktexturetest
            quicktexturetest.cpp
            quickinspectortest.qrc
        )
        target_link_libraries(quicktexturetest gammaray_core Qt5::Quick)

        gammaray_add_quick_test(quickmaterialtest
            quickmaterialtest.cpp
            quickinspectortest.qrc
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_include_directories(quickmaterialtest SYSTEM PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
        target_link_libraries(quickmaterialtest gammaray_core Qt5::Quick)
    endif()

    if(Qt5IviCore_FOUND AND Qt5IviVehicleFunctions_FOUND AND Qt5IviCore_VERSION VERSION_LESS 1.2)
        gammaray_add_probe_test(qtivipropertymodeltest
            qtivipropertymodeltest.cpp
            ../plugins/qtivi/qtivipropertyoverrider.cpp
            ../plugins/qtivi/qtivipropertymodel.cpp
            $<TARGET_OBJECTS:modeltestobj>
        )
        target_include_directories(qtivipropertymodeltest SYSTEM PRIVATE ${Qt5IviCore_PRIVATE_INCLUDE_DIRS})
        target_link_libraries(qtivipropertymodeltest Qt5::Gui Qt5::IviCore Qt5::IviVehicleFunctions)
        if(Qt5IviMedia_FOUND)
            target_link_libraries(qtivipropertymodeltest Qt5::IviMedia)
        endif()
    endif()
endif()
