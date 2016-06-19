if(UNIX)
  find_program(POD2MAN_EXECUTABLE pod2man)
  gammaray_add_dummy_package(pod2man POD2MAN_EXECUTABLE)
  set_package_properties(pod2man PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Man page generator"
    PURPOSE "Generate GammaRay man pages."
  )

  if(POD2MAN_EXECUTABLE)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
      COMMAND ${POD2MAN_EXECUTABLE} -c "KDAB" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod
    )
    add_custom_target(man ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1 DESTINATION ${MAN_INSTALL_DIR})
  endif()
endif()
