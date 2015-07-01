add_subdirectory(injector)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# shared launcher code
set(gammaray_launcher_srcs
  injector/abstractinjector.cpp
  injector/processinjector.cpp
  injector/injectorfactory.cpp
  injector/styleinjector.cpp
  injector/windllinjector.cpp
  injector/interactiveprocess.cpp

  probefinder.cpp
  launchoptions.cpp
  clientlauncher.cpp
  launcherfinder.cpp
  launcher.cpp
)

gammaray_install_headers(
    gammaray_launcher_export.h
    launcher.h
    launchoptions.h
)

if(NOT WIN32)
  list(APPEND gammaray_launcher_srcs
    injector/debuggerinjector.cpp
    injector/gdbinjector.cpp
    injector/lldbinjector.cpp
    injector/preloadcheck.cpp
    injector/preloadinjector.cpp
  )
endif()
add_library(gammaray_launcher SHARED ${gammaray_launcher_srcs})
target_link_libraries(gammaray_launcher ${QT_QTCORE_LIBRARIES} gammaray_common)
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray_launcher ${QT_QTGUI_LIBRARIES})
endif()
if(UNIX AND NOT APPLE AND NOT QNXNTO)
  target_link_libraries(gammaray_launcher dl) # for preload check
endif()

add_definitions(-DMAKE_GAMMARAY_LAUNCHER_LIB)

install(TARGETS gammaray_launcher EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_pri_file(BASE_NAME GammaRayLauncher
                      LIB_NAME gammaray_launcher
                      DEPS "core gui GammaRayCommon"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

# command line launcher
set(gammaray_runner_srcs
  main.cpp
)

add_executable(gammaray ${gammaray_runner_srcs})

target_link_libraries(gammaray gammaray_launcher gammaray_common_internal)

if(QNXNTO)
  target_link_libraries(gammaray cpp)
endif()

gammaray_embed_info_plist(gammaray ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

install(TARGETS gammaray ${INSTALL_TARGETS_DEFAULT_ARGS})

# UI launcher
if(HAVE_QT_CONCURRENT AND GAMMARAY_BUILD_UI)
  add_subdirectory(ui)
endif()
