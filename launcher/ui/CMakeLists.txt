set(gammaray_launcher_ui_internal_srcs
  ${CMAKE_SOURCE_DIR}/ui/searchlinecontroller.cpp
  promolabel.cpp
  launcherwindow.cpp
  launchpage.cpp
  selftestpage.cpp
  connectpage.cpp
  attachdialog.cpp
  processmodel.cpp
  processfiltermodel.cpp
  probeabimodel.cpp
)
if(NOT WIN32)
  list(APPEND gammaray_launcher_ui_internal_srcs processlist_unix.cpp)
else()
  list(APPEND gammaray_launcher_ui_internal_srcs processlist_win.cpp)
endif()

qt4_wrap_ui(gammaray_launcher_ui_internal_srcs
  attachdialog.ui
  launcherwindow.ui
  launchpage.ui
  selftestpage.ui
  connectpage.ui
)

add_library(gammaray_launcher_ui_internal STATIC ${gammaray_launcher_ui_internal_srcs})
target_compile_definitions(gammaray_launcher_ui_internal PRIVATE GAMMARAY_UI_STATIC_DEFINE)
target_link_libraries(gammaray_launcher_ui_internal
  LINK_PUBLIC
  gammaray_launcher

  LINK_PRIVATE
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTCONCURRENT_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTNETWORK_LIBRARIES}
  gammaray_ui
)

install(TARGETS gammaray_launcher_ui_internal EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

set(gammaray_launcher_ui_srcs main.cpp)
# TODO we don't need all the class icons here, so split the qrc file accordingly
qt4_add_resources(gammaray_launcher_ui_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_executable(gammaray-launcher WIN32 ${gammaray_launcher_ui_srcs})

target_link_libraries(gammaray-launcher
  ${QT_QTGUI_LIBRARIES}
  gammaray_common
  gammaray_launcher_ui_internal
)

gammaray_embed_info_plist(gammaray-launcher ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
set_target_properties(gammaray-launcher PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${LIBEXEC_INSTALL_DIR}"
)

set(gammaray_launcher_dialog_srcs launcherdialog.cpp)
add_library(gammaray_launcher_dialog SHARED ${gammaray_launcher_dialog_srcs})
target_link_libraries(gammaray_launcher_dialog
  ${QT_QTGUI_LIBRARIES}
  gammaray_common
  gammaray_launcher_ui_internal
)

install(TARGETS gammaray_launcher_dialog EXPORT GammaRayTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
generate_export_header(gammaray_launcher_dialog)

ecm_generate_pri_file(BASE_NAME GammaRayLauncherDialog
                    LIB_NAME gammaray_launcher_dialog
                    DEPS "core gui GammaRayCommon GammaRayLauncher"
                    FILENAME_VAR PRI_FILENAME
                    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR})

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


install(TARGETS gammaray-launcher DESTINATION ${LIBEXEC_INSTALL_DIR})

