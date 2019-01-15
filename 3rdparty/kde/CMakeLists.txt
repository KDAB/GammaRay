set(gammaray_kitemmodels_srcs
  klinkitemselectionmodel.cpp
  kmodelindexproxymapper.cpp
  krecursivefilterproxymodel.cpp
  kitemmodels_debug.cpp
)

add_library(gammaray_kitemmodels SHARED ${gammaray_kitemmodels_srcs})

target_link_libraries(gammaray_kitemmodels LINK_PUBLIC Qt5::Core)

set_target_properties(gammaray_kitemmodels PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  OUTPUT_NAME gammaray_kitemmodels-${GAMMARAY_PROBE_ABI}
  DEFINE_SYMBOL MAKE_KITEMMODELS_LIB
)
gammaray_set_rpath(gammaray_kitemmodels ${LIB_INSTALL_DIR})

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(TARGETS gammaray_kitemmodels EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
  if(MSVC)
    install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_kitemmodels>/$<TARGET_PDB_FILE_NAME:gammaray_kitemmodels>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
  endif()

  ecm_generate_pri_file(BASE_NAME GammaRayKItemModels
                        LIB_NAME gammaray_kitemmodels-${GAMMARAY_PROBE_ABI}
                        DEPS "core"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
  )

  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
else()
  install(TARGETS gammaray_kitemmodels ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()
