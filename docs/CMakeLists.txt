#
# man page
#
if (UNIX)
  find_program(POD2MAN_EXECUTABLE pod2man)
  gammaray_add_dummy_package(pod2man POD2MAN_EXECUTABLE)
  set_package_properties(pod2man PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Man page generator"
    PURPOSE "Generate GammaRay man pages."
  )

  if(POD2MAN_EXECUTABLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
      COMMAND ${POD2MAN_EXECUTABLE} -c "KDAB Products" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod
    )
    add_custom_target(man ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1 DESTINATION share/man/man1)
  endif()
endif()

#
# API docs
#
find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "API Documentation system"
  URL "http://www.doxygen.org"
  PURPOSE "Needed to build the API documention."
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps *.dox *.html)
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
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
  )

endif()
