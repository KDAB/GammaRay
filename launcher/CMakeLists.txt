add_subdirectory(injector)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}

  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

# shared launcher code
set(gammaray_launcher_shared_srcs
  injector/abstractinjector.cpp
  injector/injectorfactory.cpp
  injector/preloadinjector.cpp
  injector/styleinjector.cpp
  injector/windllinjector.cpp
  injector/interactiveprocess.cpp
  injector/preloadcheck.cpp

  probefinder.cpp
  launchoptions.cpp
  clientlauncher.cpp
  launcherfinder.cpp
)
if(NOT WIN32)
  list(APPEND gammaray_launcher_shared_srcs injector/gdbinjector.cpp)
endif()
add_library(gammaray_launcher_shared STATIC ${gammaray_launcher_shared_srcs})
target_link_libraries(gammaray_launcher_shared ${QT_QTCORE_LIBRARIES})

# command line launcher
set(gammaray_runner_srcs
  launcher.cpp
  main.cpp
)

add_executable(gammaray ${gammaray_runner_srcs})

target_link_libraries(gammaray ${QT_QTCORE_LIBRARIES} gammaray_launcher_shared gammaray_common)
if(HAVE_QT_WIDGETS)
  # needed for style inspector
  target_link_libraries(gammaray ${QT_QTGUI_LIBRARIES})
endif()

if(QNXNTO)
  target_link_libraries(gammaray cpp)
endif()

if(UNIX AND NOT APPLE AND NOT QNXNTO)
  target_link_libraries(gammaray dl) # for preload check
endif()

install(TARGETS gammaray ${INSTALL_TARGETS_DEFAULT_ARGS})

# UI launcher
if(HAVE_QT_CONCURRENT AND HAVE_QT_WIDGETS)
  add_subdirectory(ui)
endif()
