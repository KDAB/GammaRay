# Integrated launcher/client for Mac app bundles
if(APPLE)

set(gammaray_app_srcs main.cpp)

add_executable(gammaray-app MACOSX_BUNDLE ${gammaray_app_srcs})

target_link_libraries(gammaray-app
  ${QT_QTGUI_LIBRARIES}
  gammaray_common
  gammaray_launcher
  gammaray_launcher_ui_internal
  gammaray_client
)

set_target_properties(gammaray-app PROPERTIES
  OUTPUT_NAME "GammaRay"
  INSTALL_RPATH "@executable_path/../Frameworks"
  MACOSX_BUNDLE_INFO_STRING "GammaRay ${GAMMARAY_VERSION}"
  MACOSX_BUNDLE_ICON_FILE "GammaRay.icns"
  MACOSX_BUNDLE_GUI_IDENTIFIER "com.kdab.gammaray"
  MACOSX_BUNDLE_LONG_VERSION_STRING "${GAMMARAY_VERSION_STRING}"
  MACOSX_BUNDLE_BUNDLE_NAME "GammaRay"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${GAMMARAY_VERSION}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${GAMMARAY_VERSION}"
  MACOSX_BUNDLE_COPYRIGHT "Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com"
)

install(TARGETS gammaray-app ${INSTALL_TARGETS_DEFAULT_ARGS})

endif()
