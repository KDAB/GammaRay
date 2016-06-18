# TODO replace with Qt5::qhelpgenerator for Qt >= 5.7.1
find_program(QHELPGEN_EXECUTABLE qhelpgenerator)

# Don't attempt create the user manual for Qt versions < 5.4
if(Qt5Core_FOUND AND NOT Qt5Core_VERSION VERSION_LESS 5.4.0)
  add_subdirectory(manual)
else()
  add_feature_info("User Manual creation in qch format" False "Requires Qt5.4 or higher")
endif()
add_subdirectory(api)
add_subdirectory(man)
