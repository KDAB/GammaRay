# put targets in their own prefix, so we can test if GammaRay is properly loaded when installed elsewhere
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testbin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testlib)

add_executable(minimalcoreapplication minimalcoreapplication.cpp)
target_link_libraries(minimalcoreapplication ${QT_QTCORE_LIBRARIES})

if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
    add_executable(minimalwidgetapplication minimalwidgetapplication.cpp)
    target_link_libraries(minimalwidgetapplication ${QT_QTGUI_LIBRARIES})
endif()
