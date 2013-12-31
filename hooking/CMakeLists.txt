# Enable RPATH, make gammaray_probe find gammaray_core (same directory)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")

set(gammaray_probe_srcs
  probecreator.cpp
  hooks.cpp
)

if(WIN32 OR APPLE)
  set(gammaray_probe_srcs
    ${gammaray_probe_srcs}
    abstractfunctionoverwriter.cpp
    functionoverwriterfactory.cpp
    winfunctionoverwriter.cpp
    unixfunctionoverwriter.cpp
  )
endif()

# probe lib
add_library(gammaray_probe MODULE ${gammaray_probe_srcs})
target_link_libraries(gammaray_probe
  gammaray_core
)
set_target_properties(gammaray_probe PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}"
)
install(TARGETS gammaray_probe DESTINATION ${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}/)
