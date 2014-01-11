if(UNIX AND NOT APPLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  install(
    FILES GammaRay-16x16.png
    DESTINATION share/icons/hicolor/16x16/apps RENAME GammaRay.png
  )
  install(
    FILES GammaRay-32x32.png
    DESTINATION share/icons/hicolor/32x32/apps RENAME GammaRay.png
  )
  install(
    FILES GammaRay-48x48.png
    DESTINATION share/icons/hicolor/48x48/apps RENAME GammaRay.png
  )
  install(
    FILES GammaRay-128x128.png
    DESTINATION share/icons/hicolor/128x128/apps RENAME GammaRay.png
  )
  install(
    FILES GammaRay-256x256.png
    DESTINATION share/icons/hicolor/256x256/apps RENAME GammaRay.png
  )
  install(
    FILES GammaRay-512x512.png
    DESTINATION share/icons/hicolor/512x512/apps RENAME GammaRay.png
  )
endif()
