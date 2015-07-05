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

  if(QNXNTO)
    target_link_libraries(benchsuite cpp)
  endif()

### CONNECTIONTEST

  add_executable(connectiontest test_connections.cpp)

  target_link_libraries(connectiontest
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
  )

  if(QNXNTO)
    target_link_libraries(connectiontest cpp)
  endif()
endif()

add_executable(attachhelper attachhelper.cpp)

target_link_libraries(attachhelper ${QT_QTCORE_LIBRARIES})

if(QNXNTO)
  target_link_libraries(attachhelper cpp)
endif()

if(UNIX)
  add_test(connectiontest-preload
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess -unittest -i preload
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
  )
  add_test(connectiontest-style
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess -unittest -i style
    ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
  )

  set(RUN_FILTER_TESTS OFF
    CACHE BOOL "Run filter tests which fail due to a bug in QSortFilterProxyModel"
  )
  if(RUN_FILTER_TESTS)
    add_test(connectiontest-preload-filter
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray -unittest -filtertest -i preload
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
    add_test(connectiontest-preload-filter-modeltest
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray -unittest -modeltest -filtertest -i preload
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
    add_test(connectiontest-style-filter
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray -unittest -filtertest -i style
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
  endif()

  find_program(GDB_EXECUTABLE gdb)
  if(GDB_EXECUTABLE)
    add_test(connectiontest-gdb
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess -unittest -i gdb
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
    add_test(attachtest-gdb
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/attachhelper ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray gdb
      ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
    )
    if(RUN_FILTER_TESTS)
      add_test(connectiontest-gdb-filter
        ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/gammaray --inprocess -unittest -filtertest -i gdb
        ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR}/connectiontest
      )
    endif()
  endif()
endif()

### MultiSignalMapper test

add_executable(multisignalmappertest multisignalmappertest.cpp ../core/multisignalmapper.cpp)
target_link_libraries(multisignalmappertest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTTEST_LIBRARIES}
)
add_test(multisignalmappertest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multisignalmappertest)

### Probe ABI test

add_executable(probeabitest probeabitest.cpp)
target_link_libraries(probeabitest gammaray_common_internal ${QT_QTTEST_LIBRARIES})
add_test(probeabitest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/probeabitest)

add_executable(probeabidetectortest probeabidetectortest.cpp)
target_link_libraries(probeabidetectortest gammaray_common_internal ${QT_QTTEST_LIBRARIES})
add_test(probeabidetectortest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/probeabidetectortest)

### Remote model tests

if(GAMMARAY_BUILD_UI AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  add_executable(remotemodeltest
    remotemodeltest.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/qt/modeltest.cpp
    ../core/remote/remotemodelserver.cpp
  )
  target_link_libraries(remotemodeltest gammaray_core gammaray_client ${QT_QTGUI_LIBRARIES} ${QT_QTTEST_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
  add_test(NAME remotemodeltest COMMAND remotemodeltest)
endif()

### QSignalSpyCallback tests

add_executable(signalspycallbacktest
  signalspycallbacktest.cpp
  ../hooking/probecreator.cpp
)
target_link_libraries(signalspycallbacktest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME signalspycallbacktest COMMAND signalspycallbacktest)

### QAction test

if(Qt5Core_FOUND AND NOT Qt5Core_VERSION_MINOR LESS 4) # requires QHooks
add_executable(actiontest
  actiontest.cpp
  ../hooking/probecreator.cpp
  ../hooking/hooks.cpp
)
target_link_libraries(actiontest gammaray_core ${QT_QTTEST_LIBRARIES} ${QT_QTGUI_LIBRARIES})

add_test(NAME actiontest COMMAND actiontest)
endif()

### MetaObject test

add_executable(metaobjecttest metaobjecttest.cpp)
target_link_libraries(metaobjecttest gammaray_core ${QT_QTTEST_LIBRARIES})
add_test(NAME metaobjecttest COMMAND metaobjecttest)

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


add_subdirectory(manual)
