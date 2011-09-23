include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(test_connections.cpp)
add_executable(connectiontest test_connections.cpp ${connection_moc})
target_link_libraries(connectiontest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})

if(UNIX)
  add_test(connectiontest-preload ${CMAKE_BINARY_DIR}/gammaray -i preload ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-preload-filter ${CMAKE_BINARY_DIR}/gammaray -filtertest -i preload ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
  add_test(connectiontest-gdb ${CMAKE_BINARY_DIR}/gammaray -i gdb ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)
endif(UNIX)

add_test(connectiontest-style ${CMAKE_BINARY_DIR}/gammaray -i style ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)