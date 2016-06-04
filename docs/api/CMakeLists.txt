find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "API Documentation system"
  URL "http://www.doxygen.org"
  PURPOSE "Needed to build the API documention."
)
if(DOXYGEN_FOUND AND QHELPGEN_EXECUTABLE)
  file(GLOB _dox_deps *.dox *.html)
  gammaray_all_installed_headers(_all_hdrs)
  list(APPEND _all_hdrs ${_dox_deps})
  gammaray_join_list(DOXYGEN_INPUT " " ${_all_hdrs})
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

  #apidox generation using doxygen
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )

  add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html
  )

endif()

