include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

add_library(gammaray_inprocessui MODULE main.cpp)
target_link_libraries(gammaray_inprocessui gammaray_core gammaray_ui_internal)
set_target_properties(gammaray_inprocessui PROPERTIES PREFIX "")
install(TARGETS gammaray_inprocessui ${INSTALL_TARGETS_DEFAULT_ARGS})
