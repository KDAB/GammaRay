include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(test_connections.cpp)
add_executable(connectiontest test_connections.cpp)
target_link_libraries(connectiontest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})

qt4_automoc(attachhelper.cpp)
add_executable(attachhelper attachhelper.cpp)
target_link_libraries(attachhelper ${QT_QTCORE_LIBRARY})

if(UNIX AND NOT APPLE)
  add_test(connectiontest-preload ${CMAKE_BINARY_DIR}/gammaray -unittest -i preload ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-preload-filter ${CMAKE_BINARY_DIR}/gammaray -unittest -filtertest -i preload ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-preload-filter-modeltest ${CMAKE_BINARY_DIR}/gammaray -unittest -modeltest -filtertest -i preload ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-gdb ${CMAKE_BINARY_DIR}/gammaray -unittest -i gdb ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-gdb-filter ${CMAKE_BINARY_DIR}/gammaray -unittest -filtertest -i gdb ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-style ${CMAKE_BINARY_DIR}/gammaray -unittest -i style ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-style-filter ${CMAKE_BINARY_DIR}/gammaray -unittest -filtertest -i style ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)

  add_test(attachtest-gdb ${CMAKE_CURRENT_BINARY_DIR}/attachhelper ${CMAKE_BINARY_DIR}/gammaray gdb ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
endif(UNIX AND NOT APPLE)
