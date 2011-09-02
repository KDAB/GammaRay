include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(endoscope_injector_style_srcs injectorstyleplugin.cpp)
qt4_automoc(${endoscope_injector_style_srcs})

add_library(endoscope_injector_style MODULE ${endoscope_injector_style_srcs})
target_link_libraries(endoscope_injector_style
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)
set_target_properties(endoscope_injector_style PROPERTIES PREFIX "")
install(
  TARGETS endoscope_injector_style
  DESTINATION ${LIB_INSTALL_DIR}/qt4/plugins/styles
)
