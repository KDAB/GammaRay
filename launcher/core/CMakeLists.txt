add_subdirectory(injector)

set(gammaray_launcher_shared_srcs
  injector/abstractinjector.cpp
  injector/processinjector.cpp
  injector/injectorfactory.cpp
  injector/styleinjector.cpp

  libraryutil.cpp
  probeabi.cpp
  probeabidetector.cpp
  probefinder.cpp
  launchoptions.cpp
  networkdiscoverymodel.cpp
  clientlauncher.cpp
  launcherfinder.cpp
  launcher.cpp
  selftest.cpp
)

if(WIN32)
  list(APPEND gammaray_launcher_shared_srcs
    probeabidetector_win.cpp
    pefile.cpp
    injector/basicwindllinjector.cpp
    injector/windllinjector.cpp
  )
else()
  list(APPEND gammaray_launcher_shared_srcs
    injector/debuggerinjector.cpp
    injector/gdbinjector.cpp
    injector/lldbinjector.cpp
    injector/preloadinjector.cpp
  )
  if(APPLE)
    list(APPEND gammaray_launcher_shared_srcs probeabidetector_mac.cpp)
  elseif(UNIX)
    list(APPEND gammaray_launcher_shared_srcs probeabidetector_elf.cpp)
  else()
    list(APPEND gammaray_launcher_shared_srcs probeabidetector_dummy.cpp)
  endif()
endif()

add_library(gammaray_launcher SHARED ${gammaray_launcher_shared_srcs})
generate_export_header(gammaray_launcher)
set_target_properties(gammaray_launcher PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
)
gammaray_set_rpath(gammaray_launcher ${LIB_INSTALL_DIR})
target_compile_features(gammaray_launcher PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

gammaray_target_relocatable_interfaces(gammaray_launcher_ipaths)
target_include_directories(gammaray_launcher PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:${gammaray_launcher_ipaths}>)
target_link_libraries(gammaray_launcher LINK_PUBLIC Qt::Core LINK_PRIVATE gammaray_common Qt::Network ${CMAKE_DL_LIBS})
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray_launcher LINK_PRIVATE Qt::Gui Qt::Widgets)
endif()
if(WIN32)
  target_link_libraries(gammaray_launcher LINK_PRIVATE version)
elseif(APPLE)
  target_link_libraries(gammaray_launcher LINK_PRIVATE "-framework CoreFoundation")
endif()

install(TARGETS gammaray_launcher EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

gammaray_install_headers(DESTINATION launcher
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_launcher_export.h
    launcher.h
    launchoptions.h
    launcherfinder.h
    probeabi.h
    probeabidetector.h
    probefinder.h
    networkdiscoverymodel.h
)

ecm_generate_pri_file(BASE_NAME GammaRayLauncher
                      LIB_NAME gammaray_launcher
                      DEPS "core gui GammaRayCommon"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_launcher>/$<TARGET_PDB_FILE_NAME:gammaray_launcher>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
