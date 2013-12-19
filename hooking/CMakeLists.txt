# Enable RPATH, make gammaray_probe find gammaray_core (same directory)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR} # config-gammaray.h
  ${CMAKE_CURRENT_BINARY_DIR}
)

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
add_library(gammaray_probe SHARED ${gammaray_probe_srcs})
target_link_libraries(gammaray_probe
  gammaray_core
)
set_target_properties(gammaray_probe PROPERTIES PREFIX "")
install(TARGETS gammaray_probe DESTINATION ${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}/)
