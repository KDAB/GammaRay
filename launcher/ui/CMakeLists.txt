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
target_include_directories(gammaray_launcher PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
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
