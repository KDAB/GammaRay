set(gammaray_probe_srcs
  probecreator.cpp
  hooks.cpp
)

if(UNIX AND Qt5Core_VERSION VERSION_LESS 5.4.0)
  list(APPEND gammaray_probe_srcs preload.cpp)
endif()

if(WIN32 OR APPLE)
  list(APPEND gammaray_probe_srcs
    abstractfunctionoverwriter.cpp
    functionoverwriterfactory.cpp
    unixfunctionoverwriter.cpp
  )
endif()

if(WIN32)
  list(APPEND gammaray_probe_srcs
    entry_win.cpp
    winfunctionoverwriter.cpp
  )
endif()

if(APPLE OR NOT Qt5Core_VERSION VERSION_LESS 5.4.0)
  list(APPEND gammaray_probe_srcs entry_unix.cpp)
endif()

# probe lib
if(APPLE) # DYLD_INSERT_LIBRARIES only works with real shared libraries, not modules
  add_library(gammaray_probe SHARED ${gammaray_probe_srcs})
  set_target_properties(gammaray_probe PROPERTIES INSTALL_RPATH "@loader_path/../../../Frameworks")
else()
  add_library(gammaray_probe MODULE ${gammaray_probe_srcs})
endif()
target_link_libraries(gammaray_probe
  gammaray_core
)

if(NOT WIN32 AND NOT QNXNTO AND NOT ANDROID)
  target_link_libraries(gammaray_probe dl)
endif()

if(NOT ANDROID)
  set_target_properties(gammaray_probe PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${PROBE_PLUGIN_INSTALL_DIR}"
  )
  set_target_properties(gammaray_probe PROPERTIES PREFIX "")
endif()

if(ANDROID)
  install(TARGETS gammaray_probe EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
  ecm_generate_pri_file(BASE_NAME GammaRayProbe
                      LIB_NAME gammaray_probe
                      DEPS "core gui network GammaRayCommon GammaRayCore"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..)
  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_probe-android-dependencies.xml DESTINATION ${LIB_INSTALL_DIR})
else()
  install(TARGETS gammaray_probe DESTINATION ${PROBE_PLUGIN_INSTALL_DIR})
endif()
