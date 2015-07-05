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
  injector/interactiveprocess.cpp

  probefinder.cpp
  launchoptions.cpp
  clientlauncher.cpp
  launcherfinder.cpp
  launcher.cpp
)
if(NOT WIN32)
  list(APPEND gammaray_launcher_shared_srcs
    injector/debuggerinjector.cpp
    injector/gdbinjector.cpp
    injector/lldbinjector.cpp
    injector/preloadcheck.cpp
    injector/preloadinjector.cpp
  )
endif()
add_library(gammaray_launcher_shared STATIC ${gammaray_launcher_shared_srcs})
target_link_libraries(gammaray_launcher_shared ${QT_QTCORE_LIBRARIES} gammaray_common_internal)
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray_launcher_shared ${QT_QTGUI_LIBRARIES})
endif()
if(UNIX AND NOT APPLE AND NOT QNXNTO)
  target_link_libraries(gammaray_launcher_shared dl) # for preload check
endif()

# command line launcher
set(gammaray_runner_srcs
  main.cpp
)

add_executable(gammaray ${gammaray_runner_srcs})

target_link_libraries(gammaray gammaray_launcher_shared)

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
