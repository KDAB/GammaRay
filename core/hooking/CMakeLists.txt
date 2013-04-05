# Enable RPATH, make gammaray_probe find gammaray_core (same directory)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories(
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

# for the hooks, we actually use gammaray_core
remove_definitions(-DMAKE_GAMMARAY_LIB)

# probe lib
qt4_automoc(${gammaray_probe_srcs})
add_library(gammaray_probe SHARED ${gammaray_probe_srcs})
target_link_libraries(gammaray_probe
  gammaray_core
)
set_target_properties(gammaray_probe PROPERTIES PREFIX "")
install(TARGETS gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
