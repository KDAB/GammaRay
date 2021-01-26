if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_probe_srcs
  probecreator.cpp
  hooks.cpp
)

list(APPEND gammaray_probe_srcs entry_unix.cpp)

# create intermediate gammaray_probe object files which unit tests can use
add_library(gammaray_probe_obj OBJECT ${gammaray_probe_srcs})
set_target_properties(gammaray_probe_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(gammaray_probe_obj PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})
target_compile_definitions(gammaray_probe_obj PRIVATE ${QtCore_DEFINITIONS}) # for -DQT_NAMESPACE=...
target_include_directories(gammaray_probe_obj PUBLIC
    $<TARGET_PROPERTY:gammaray_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:gammaray_common,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(gammaray_probe_obj Qt::CorePrivate)

# probe lib
# DYLD_INSERT_LIBRARIES only works with real shared libraries, not modules
# Windows also needs SHARED rather than MODULE here, to end up in bin/
add_library(gammaray_probe ${GAMMARAY_LIBRARY_TYPE} $<TARGET_OBJECTS:gammaray_probe_obj>)
target_link_libraries(gammaray_probe gammaray_core ${CMAKE_DL_LIBS})

set_target_properties(gammaray_probe PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
)
target_compile_features(gammaray_probe PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

if(WIN32)
  add_library(gammaray_winloader SHARED entry_win.cpp)
  set_target_properties(gammaray_winloader PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${GAMMARAY_OUTPUT_PREFIX}/${PROBE_INSTALL_DIR}"
  )
    target_compile_features(gammaray_winloader PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})
endif()

if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
  set_target_properties(gammaray_probe PROPERTIES PREFIX "")
  gammaray_set_rpath(gammaray_probe ${PROBE_INSTALL_DIR})
  install(TARGETS gammaray_probe EXPORT GammaRayTargets DESTINATION ${PROBE_INSTALL_DIR})
  if(WIN32)
    set_target_properties(gammaray_winloader PROPERTIES PREFIX "")
    install(TARGETS gammaray_winloader DESTINATION ${PROBE_INSTALL_DIR})
  endif()
else()
  set_target_properties(gammaray_probe PROPERTIES OUTPUT_NAME gammaray_probe-${GAMMARAY_PROBE_ABI})
  gammaray_set_rpath(gammaray_probe ${LIB_INSTALL_DIR})
  if(WIN32)
    set_target_properties(gammaray_winloader PROPERTIES OUTPUT_NAME gammaray_winloader-${GAMMARAY_PROBE_ABI})
    install(TARGETS gammaray_winloader ${INSTALL_TARGETS_DEFAULT_ARGS})
  endif()
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    install(TARGETS gammaray_probe EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

    ecm_generate_pri_file(BASE_NAME GammaRayProbe
                        LIB_NAME gammaray_probe-${GAMMARAY_PROBE_ABI}
                        DEPS "core gui network GammaRayCommon GammaRayCore"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR})

    install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
  else()
    install(TARGETS gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
  endif()
  if (ANDROID)
    if (QtCore_VERSION VERSION_LESS 5.14.0)
      install(FILES gammaray_probe-android-dependencies.xml RENAME gammaray_probe-${GAMMARAY_PROBE_ABI}-android-dependencies.xml DESTINATION ${LIB_INSTALL_DIR})
    else()
      install(FILES gammaray_probe-android-dependencies.xml RENAME gammaray_probe-${GAMMARAY_PROBE_ABI}_${ANDROID_ABI}-android-dependencies.xml DESTINATION ${LIB_INSTALL_DIR})
    endif()
  endif()
endif()

if(MSVC)
    install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_probe>/$<TARGET_PDB_FILE_NAME:gammaray_probe>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()

endif() # GAMMARAY_CLIENT_ONLY_BUILD
