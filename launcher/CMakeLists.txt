add_subdirectory(injector)

if(NOT GAMMARAY_PROBE_ONLY_BUILD)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# shared launcher code
set(gammaray_launcher_shared_srcs
  injector/abstractinjector.cpp
  injector/processinjector.cpp
  injector/injectorfactory.cpp
  injector/styleinjector.cpp
  injector/windllinjector.cpp

  probeabi.cpp
  probeabidetector.cpp
  probefinder.cpp
  launchoptions.cpp
  clientlauncher.cpp
  launcherfinder.cpp
  launcher.cpp
)

if(WIN32)
  list(APPEND gammaray_launcher_shared_srcs probeabidetector_win.cpp pefile.cpp)
else()
  list(APPEND gammaray_launcher_shared_srcs
    injector/debuggerinjector.cpp
    injector/gdbinjector.cpp
    injector/lldbinjector.cpp
    injector/preloadcheck.cpp
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

set_target_properties(gammaray_launcher PROPERTIES
  ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES}
  DEFINE_SYMBOL MAKE_GAMMARAY_LAUNCHER_LIB
)

target_link_libraries(gammaray_launcher LINK_PUBLIC ${QT_QTCORE_LIBRARIES} LINK_PRIVATE gammaray_common)
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray_launcher LINK_PRIVATE ${QT_QTGUI_LIBRARIES})
endif()
if(WIN32)
  target_link_libraries(gammaray_launcher LINK_PRIVATE version)
elseif(APPLE)
  target_link_libraries(gammaray_launcher LINK_PRIVATE "-framework CoreFoundation")
endif()
if(UNIX AND NOT APPLE AND NOT QNXNTO)
  target_link_libraries(gammaray_launcher LINK_PRIVATE dl) # for preload check
endif()

install(TARGETS gammaray_launcher EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_pri_file(BASE_NAME GammaRayLauncher
                      LIB_NAME gammaray_launcher
                      DEPS "core gui GammaRayCommon"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/..
)

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
gammaray_install_headers(
    gammaray_launcher_export.h
    launcher.h
    launchoptions.h
    probeabi.h
    probeabidetector.h
)


# command line launcher
set(gammaray_runner_srcs
  main.cpp
)

add_executable(gammaray ${gammaray_runner_srcs})

target_link_libraries(gammaray gammaray_launcher gammaray_common)
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray ${QT_QTGUI_LIBRARIES})
endif()
if(QNXNTO)
  target_link_libraries(gammaray cpp)
endif()

gammaray_embed_info_plist(gammaray ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

install(TARGETS gammaray ${INSTALL_TARGETS_DEFAULT_ARGS})

# UI launcher
if(HAVE_QT_CONCURRENT AND GAMMARAY_BUILD_UI)
  add_subdirectory(ui)
endif()
endif()
