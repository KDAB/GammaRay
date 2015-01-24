# Integrated launcher/client for Mac app bundles
if(APPLE)

set(gammaray_app_srcs main.cpp)
qt4_add_resources(gammaray_app_srcs ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc)

add_executable(gammaray-app ${gammaray_app_srcs})
target_link_libraries(gammaray-app
  ${QT_QTGUI_LIBRARIES}
  gammaray_common_internal
  gammaray_launcher_shared
  gammaray_launcher_ui_internal
  gammaray_client_internal
)

endif()
