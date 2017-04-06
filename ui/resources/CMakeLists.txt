if(UNIX AND NOT APPLE AND GAMMARAY_BUILD_UI)
  install(
    FILES gammaray/GammaRay-16x16.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/16x16/apps" RENAME GammaRay.png
  )
  install(
    FILES gammaray/GammaRay-32x32.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/32x32/apps" RENAME GammaRay.png
  )
  install(
    FILES gammaray/GammaRay-48x48.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/48x48/apps" RENAME GammaRay.png
  )
  install(
    FILES gammaray/GammaRay-128x128.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/128x128/apps" RENAME GammaRay.png
  )
  install(
    FILES gammaray/GammaRay-256x256.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/256x256/apps" RENAME GammaRay.png
  )
  install(
    FILES gammaray/GammaRay-512x512.png
    DESTINATION "${ICON_INSTALL_DIR}/hicolor/512x512/apps" RENAME GammaRay.png
  )
endif()

if(APPLE)
  if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
    install(FILES gammaray/GammaRay.icns DESTINATION "${RESOURCES_INSTALL_DIR}")
  endif()
endif()
