set(gammaray_launcher_ui_srcs
  promolabel.cpp
  launcherwindow.cpp
  launchpage.cpp
  selftestpage.cpp
  connectpage.cpp
  attachdialog.cpp
  processmodel.cpp
  processfiltermodel.cpp
  probeabimodel.cpp
  resources/ui.qrc
)
if(NOT WIN32)
  list(APPEND gammaray_launcher_ui_srcs processlist_unix.cpp)
else()
  list(APPEND gammaray_launcher_ui_srcs processlist_win.cpp)
endif()

add_library(gammaray_launcher_ui SHARED ${gammaray_launcher_ui_srcs})
generate_export_header(gammaray_launcher_ui)
set_target_properties(gammaray_launcher_ui PROPERTIES ${GAMMARAY_DEFAULT_LIBRARY_PROPERTIES})
gammaray_set_rpath(gammaray_launcher_ui ${LIB_INSTALL_DIR})
target_compile_features(gammaray_launcher_ui PUBLIC ${GAMMARAY_REQUIRED_CXX_FEATURES})

target_include_directories(gammaray_launcher_ui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_link_libraries(gammaray_launcher_ui
  LINK_PUBLIC
  gammaray_launcher

  LINK_PRIVATE
  Qt5::Core
  Qt5::Concurrent
  Qt5::Gui Qt5::Widgets
  Qt5::Network
  gammaray_ui
)

install(TARGETS gammaray_launcher_ui EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

gammaray_install_headers(DESTINATION launcher
    ${CMAKE_CURRENT_BINARY_DIR}/gammaray_launcher_ui_export.h
    connectpage.h
    launcherwindow.h
    attachdialog.h
)

ecm_generate_pri_file(BASE_NAME GammaRayLauncherUi
                      LIB_NAME gammaray_launcher_ui
                      DEPS "core gui GammaRayLauncher"
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray_launcher_ui>/$<TARGET_PDB_FILE_NAME:gammaray_launcher_ui>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
