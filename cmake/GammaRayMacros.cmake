#
# GAMMARAY_ADD_PLUGIN: create a gammaray plugin, install at the right place, etc
#

macro(gammaray_add_plugin _target_name _desktop_file)
  add_library(${_target_name} MODULE ${ARGN})
  set_target_properties(${_target_name} PROPERTIES 
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/plugins/gammaray
  )
  install(TARGETS ${_target_name} DESTINATION ${PLUGIN_INSTALL_DIR}/gammaray/)
  install(FILES ${_desktop_file} DESTINATION ${PLUGIN_INSTALL_DIR}/gammaray/)
  #execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/${_desktop_file}" "${PROJECT_BINARY_DIR}/lib/plugins/gammaray")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${_desktop_file}" DESTINATION "${PROJECT_BINARY_DIR}/lib/plugins/gammaray")
endmacro()
