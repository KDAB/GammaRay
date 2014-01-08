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
if(APPLE) # DYLD_INSERT_LIBRARIES only works with real shared libraries, not modules
  add_library(gammaray_probe SHARED ${gammaray_probe_srcs})
else()
  add_library(gammaray_probe MODULE ${gammaray_probe_srcs})
endif()
target_link_libraries(gammaray_probe
  gammaray_core
)

if(NOT WIN32 AND NOT QNXNTO)
  target_link_libraries(gammaray_probe dl)
endif()

set_target_properties(gammaray_probe PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}"
)
install(TARGETS gammaray_probe DESTINATION ${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}/)
