# put targets in their own prefix, so we can test if GammaRay is properly loaded when installed elsewhere
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testbin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/testlib)

add_executable(minimalcoreapplication minimalcoreapplication.cpp)
target_link_libraries(minimalcoreapplication Qt5::Core)

if(Qt5Widgets_FOUND)
    add_executable(minimalwidgetapplication minimalwidgetapplication.cpp)
    target_link_libraries(minimalwidgetapplication Qt5::Gui Qt5::Widgets)
endif()
