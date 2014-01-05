set(gammaray_launcher_ui_srcs
  main.cpp
  promolabel.cpp
  launcherwindow.cpp
  launchpage.cpp
  selftestpage.cpp
  connectpage.cpp
  networkdiscoverymodel.cpp
  attachdialog.cpp
  processmodel.cpp
  processfiltermodel.cpp
)
if(NOT WIN32)
  list(APPEND gammaray_launcher_ui_srcs processlist_unix.cpp)
else()
  list(APPEND gammaray_launcher_ui_srcs processlist_win.cpp)
endif()

qt4_wrap_ui(gammaray_launcher_ui_srcs
  attachdialog.ui
  launcherwindow.ui
  launchpage.ui
  selftestpage.ui
  connectpage.ui
)
# TODO we don't need all the class icons here, so split the qrc file accordingly
qt4_add_resources(gammaray_launcher_ui_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_executable(gammaray-launcher WIN32 ${gammaray_launcher_ui_srcs})

target_link_libraries(gammaray-launcher
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTCONCURRENT_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  gammaray_common_internal
  gammaray_ui
  gammaray_launcher_shared
)

if(QNXNTO)
  target_link_libraries(gammaray-launcher cpp)
endif()

if(UNIX AND NOT APPLE AND NOT QNXNTO)
  target_link_libraries(gammaray-launcher dl) # for preload check
endif()

set_target_properties(gammaray-launcher PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${LIBEXEC_INSTALL_DIR}"
)

install(TARGETS gammaray-launcher DESTINATION ${LIBEXEC_INSTALL_DIR})

