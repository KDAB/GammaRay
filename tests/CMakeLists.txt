include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(test_connections.cpp)
add_executable(connectiontest test_connections.cpp ${connection_moc})
target_link_libraries(connectiontest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
add_test(connectiontest ${CMAKE_BINARY_DIR}/endoscope ${CMAKE_CURRENT_BINARY_DIR}/connectiontest)