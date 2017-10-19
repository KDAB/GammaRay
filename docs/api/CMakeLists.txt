find_file(QDOC_QTCORE_TAG qtcore.tags
  HINTS ${QT_INSTALL_DOCS}/qtcore
  HINTS ${QT_INSTALL_DATA}/doc/qtcore
)
if(QDOC_QTCORE_TAG)
  get_filename_component(QDOC_TAG_DIR ${QDOC_QTCORE_TAG} DIRECTORY)
  get_filename_component(QDOC_TAG_DIR ${QDOC_TAG_DIR} DIRECTORY)
endif()

file(GLOB _dox_deps *.dox *.html)
gammaray_all_installed_headers(_all_hdrs)
list(APPEND _all_hdrs ${_dox_deps})
gammaray_join_list(DOXYGEN_INPUT " \\\\\n" ${_all_hdrs})
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

#apidox generation using doxygen
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
)

add_custom_command(
  OUTPUT ${DOXYGEN_OUTPUT_DIR}/qch/gammaray-api.qch
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(gammaray-api.qch ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/qch/gammaray-api.qch)

install(FILES ${DOXYGEN_OUTPUT_DIR}/qch/gammaray-api.qch DESTINATION ${QCH_INSTALL_DIR})
