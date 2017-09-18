if(Qt5Core_FOUND)
  find_package(KF5CoreAddons NO_MODULE QUIET)
  set_package_properties(KF5CoreAddons PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "KDE KCoreAddons Framework"
    URL "https://www.kde.org/"
    PURPOSE "Required for the KJob tracker plug-in."
  )
else()
  # ugly workaround for FindKDE4Internal messing with the install rpath (https://github.com/KDAB/GammaRay/issues/36)
  set(_old_CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH})
  find_package(KDE4)
  set(CMAKE_INSTALL_RPATH ${_old_CMAKE_INSTALL_RPATH})

  set_package_properties(KDE4 PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "KDE libraries"
    URL "https://www.kde.org/"
    PURPOSE "Required for the KJob tracker plug-in."
  )
endif()

# probe part
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
if(KDE4_FOUND OR KF5CoreAddons_FOUND)
  set(gammaray_kjob_plugin_srcs
    kjobtracker.cpp
    kjobmodel.cpp
  )

  gammaray_add_plugin(gammaray_kjobtracker_plugin
    DESKTOP gammaray_kjobtracker.desktop.in
    JSON gammaray_kjobtracker.json
    SOURCES ${gammaray_kjob_plugin_srcs}
  )

  if(KDE4_FOUND)
    target_compile_definitions(gammaray_kjobtracker_plugin PRIVATE ${KDE4_DEFINITIONS})
    target_include_directories(gammaray_kjobtracker_plugin PRIVATE ${KDE4_INCLUDES})
    target_link_libraries(gammaray_kjobtracker_plugin
      ${KDE4_KDECORE_LIBS}
      gammaray_core
    )
  else()
    target_link_libraries(gammaray_kjobtracker_plugin
      gammaray_core
      KF5::CoreAddons
      Qt5::Gui
    )
  endif()
endif()
endif()

# ui part
if(GAMMARAY_BUILD_UI)
  set(gammaray_kjob_ui_plugin_srcs
    kjobtrackerwidget.cpp
  )

  gammaray_add_plugin(gammaray_kjobtracker_ui_plugin
    DESKTOP gammaray_kjobtracker_ui.desktop.in
    JSON gammaray_kjobtracker.json
    SOURCES ${gammaray_kjob_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_kjobtracker_ui_plugin
    gammaray_ui
  )
endif()
