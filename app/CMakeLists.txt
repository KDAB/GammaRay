# Integrated launcher/client for Mac app bundles
if(APPLE)
  if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
    # CMake does not allow to have a bundle executable having a name different then the bundle folder name...
    # So we are disabling the bundle behavior here and generate a standard binary
    # The bundle structure will be auto created from the different INSTALL_XXX variables and we only manually generate
    # the Info.plist file

    set(gammaray_app_srcs main.cpp)

    add_executable(gammaray-app ${gammaray_app_srcs})

    target_link_libraries(gammaray-app
      Qt::Gui Qt::Widgets
      gammaray_common
      gammaray_launcher
      gammaray_launcher_ui
      gammaray_client
    )

    set_target_properties(gammaray-app PROPERTIES
      OUTPUT_NAME "GammaRayUI"
      INSTALL_RPATH "@executable_path/../Frameworks"
    )

    # Let generate our Info.plist file...
    set(GAMMARAY_BUNDLE_INFO_PLIST_IN "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")
    set(GAMMARAY_BUNDLE_INFO_PLIST_OUT "${PROJECT_BINARY_DIR}/${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/Info.plist")

    set(GAMMARAY_BUNDLE_GUI_IDENTIFIER "com.kdab.gammaray")
    set(GAMMARAY_BUNDLE_EXECUTABLE "GammaRayUI")
    set(GAMMARAY_BUNDLE_BUNDLE_NAME "GammaRay")
    set(GAMMARAY_BUNDLE_ICON_FILE "GammaRay.icns")
    set(GAMMARAY_BUNDLE_BUNDLE_VERSION "${GAMMARAY_VERSION}")
    set(GAMMARAY_BUNDLE_SHORT_VERSION_STRING "${GAMMARAY_VERSION}")
    set(GAMMARAY_BUNDLE_LONG_VERSION_STRING "${GAMMARAY_VERSION_STRING}")
    set(GAMMARAY_BUNDLE_INFO_STRING "GammaRay ${GAMMARAY_VERSION}")
    set(GAMMARAY_BUNDLE_COPYRIGHT "Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com")

    configure_file("${GAMMARAY_BUNDLE_INFO_PLIST_IN}" "${GAMMARAY_BUNDLE_INFO_PLIST_OUT}")

    # Install stuff
    install(TARGETS gammaray-app ${INSTALL_TARGETS_DEFAULT_ARGS})
    install(FILES "${GAMMARAY_BUNDLE_INFO_PLIST_OUT}" DESTINATION "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents")
  endif()
endif()
