if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
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

# create intermediate gammaray_probe object files which unit tests can use
add_library(gammaray_probe_obj OBJECT ${gammaray_probe_srcs})
set_target_properties(gammaray_probe_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(gammaray_probe_obj PRIVATE ${Qt5Core_DEFINITIONS}) # for -DQT_NAMESPACE=...
target_include_directories(gammaray_probe_obj PUBLIC
    $<TARGET_PROPERTY:gammaray_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:gammaray_common,INTERFACE_INCLUDE_DIRECTORIES>
)
target_include_directories(gammaray_probe_obj SYSTEM PRIVATE ${Qt5Core_PRIVATE_INCLUDE_DIRS})

# probe lib
# DYLD_INSERT_LIBRARIES only works with real shared libraries, not modules
# Windows also needs SHARED rather than MODULE here, to end up in bin/
add_library(gammaray_probe ${GAMMARAY_LIBRARY_TYPE} $<TARGET_OBJECTS:gammaray_probe_obj>)
if(APPLE AND NOT GAMMARAY_INSTALL_QT_LAYOUT)
    set_target_properties(gammaray_probe PROPERTIES INSTALL_RPATH "@loader_path/../../../../Frameworks")
endif()
target_link_libraries(gammaray_probe gammaray_core ${DL_LIBRARY})

set_target_properties(gammaray_probe PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
)

if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
  set_target_properties(gammaray_probe PROPERTIES PREFIX "")
  install(TARGETS gammaray_probe DESTINATION ${PROBE_INSTALL_DIR})
  if(MSVC)
    install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_core>/$<TARGET_PDB_FILE_NAME:gammaray_core>" DESTINATION ${PROBE_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
  endif()
else()
  set_target_properties(gammaray_probe PROPERTIES OUTPUT_NAME gammaray_probe-${GAMMARAY_PROBE_ABI})

  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(TARGETS gammaray_probe EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

    ecm_generate_pri_file(BASE_NAME GammaRayProbe
                        LIB_NAME gammaray_probe-${GAMMARAY_PROBE_ABI}
                        DEPS "core gui network GammaRayCommon GammaRayCore"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR})

    install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_probe-android-dependencies.xml DESTINATION ${LIB_INSTALL_DIR})
  else()
    install(TARGETS gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
  endif()
      if(MSVC)
      install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_core>/$<TARGET_PDB_FILE_NAME:gammaray_core>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
    endif()
endif()
endif()
