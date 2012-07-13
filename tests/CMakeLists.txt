include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(test_connections.cpp)

add_executable(connectiontest test_connections.cpp)

target_link_libraries(connectiontest
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTTEST_LIBRARIES}
)

qt4_automoc(attachhelper.cpp)

add_executable(attachhelper attachhelper.cpp)

target_link_libraries(attachhelper ${QT_QTCORE_LIBRARIES})

if(UNIX AND NOT APPLE)
  add_test(connectiontest-preload
    ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -i preload
    ${PROJECT_BINARY_DIR}/bin/connectiontest
  )
  add_test(connectiontest-style
    ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -i style
    ${PROJECT_BINARY_DIR}/bin/connectiontest
  )

  set(RUN_FILTER_TESTS OFF
    CACHE BOOL "Run filter tests which fail due to a bug in QSortFilterProxyModel"
  )
  if(RUN_FILTER_TESTS)
    add_test(connectiontest-preload-filter
      ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -filtertest -i preload
      ${PROJECT_BINARY_DIR}/bin/connectiontest
    )
    add_test(connectiontest-preload-filter-modeltest
      ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -modeltest -filtertest -i preload
      ${PROJECT_BINARY_DIR}/bin/connectiontest
    )
    add_test(connectiontest-style-filter
      ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -filtertest -i style
      ${PROJECT_BINARY_DIR}/bin/connectiontest
    )
  endif()

  find_program(GDB_EXECUTABLE gdb)
  if(GDB_EXECUTABLE)
    add_test(connectiontest-gdb
      ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -i gdb
      ${PROJECT_BINARY_DIR}/bin/connectiontest
    )
    add_test(attachtest-gdb
      ${PROJECT_BINARY_DIR}/bin/attachhelper ${PROJECT_BINARY_DIR}/bin/gammaray gdb
      ${PROJECT_BINARY_DIR}/bin/connectiontest
    )
    if(RUN_FILTER_TESTS)
      add_test(connectiontest-gdb-filter
        ${PROJECT_BINARY_DIR}/bin/gammaray -unittest -filtertest -i gdb
        ${PROJECT_BINARY_DIR}/bin/connectiontest
      )
    endif()
  endif()
endif()

add_subdirectory(manual)
