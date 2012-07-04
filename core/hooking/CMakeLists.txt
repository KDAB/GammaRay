# Enable RPATH, make gammaray_probe find gammaray_core (same directory)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(gammaray_probe_srcs
  abstractfunctionoverwriter.cpp
  functionoverwriterfactory.cpp
  winfunctionoverwriter.cpp
  unixfunctionoverwriter.cpp
  probecreator.cpp
  hooks.cpp
)

# probe lib
qt4_automoc(${gammaray_probe_srcs})
add_library(gammaray_probe SHARED ${gammaray_probe_srcs})
target_link_libraries(gammaray_probe
  gammaray_core
)
set_target_properties(gammaray_probe PROPERTIES PREFIX "")
install(TARGETS gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
