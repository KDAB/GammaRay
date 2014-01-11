find_package(Doxygen)
macro_log_feature(
  DOXYGEN_FOUND
  "Doxygen"
  "Documentation system"
  "http://www.doxygen.org"
  FALSE
  ""
  "Needed to build the optional API documention"
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps docs/*.dox docs/*.html)
  gammaray_all_installed_headers(_all_hdrs)
  list(APPEND _all_hdrs ${_dox_deps})
  gammaray_join_list(DOXYGEN_INPUT " " ${_all_hdrs})

  #apidox generation using doxygen
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/apidocs/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
  )

endif()
