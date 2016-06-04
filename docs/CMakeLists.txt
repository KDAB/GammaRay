# TODO replace with Qt5::qhelpgenerator for Qt >= 5.7.1
find_program(QHELPGEN_EXECUTABLE qhelpgenerator)

#
# man page
#
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
      COMMAND ${POD2MAN_EXECUTABLE} -c "KDAB Products" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.pod
    )
    add_custom_target(man ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.1 DESTINATION ${MAN_INSTALL_DIR})
  endif()
endif()

# Don't attempt create the user manual for Qt versions < 5.4
if(Qt5Core_FOUND AND NOT Qt5Core_VERSION VERSION_LESS 5.4.0)
  add_subdirectory(manual)
else()
  add_feature_info("User Manual creation in qch format" False "Requires Qt5.4 or higher")
endif()
add_subdirectory(api)
