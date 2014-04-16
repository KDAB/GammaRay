if(Qt5Core_FOUND)
  find_package(KF5CoreAddons NO_MODULE QUIET)
  set_package_properties(KF5CoreAddons PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "KDE KCoreAddons Framework"
    URL "http://www.kde.org/"
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
    URL "http://www.kde.org/"
    PURPOSE "Required for the KJob tracker plug-in."
  )
endif()


# probe part
if(KDE4_FOUND OR KF5CoreAddons_FOUND)
  include_directories(${KDE4_INCLUDES})
  add_definitions(${KDE4_DEFINITIONS})

  set(gammaray_kjob_plugin_srcs
    kjobmodel.cpp
    kjobtracker.cpp
  )

  gammaray_add_plugin(gammaray_kjobtracker_plugin
    gammaray_kjobtracker.desktop
    ${gammaray_kjob_plugin_srcs}
  )

  if (KDE4_FOUND)
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

# ui part
if((Qt5Widgets_FOUND OR QT_QTGUI_FOUND) AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  set(gammaray_kjob_ui_plugin_srcs
    kjobtrackerwidget.cpp
  )

  qt4_wrap_ui(gammaray_kjob_ui_plugin_srcs
    kjobtrackerwidget.ui
  )

  gammaray_add_plugin(gammaray_kjobtracker_ui_plugin
    gammaray_kjobtracker_ui.desktop
    ${gammaray_kjob_ui_plugin_srcs}
  )

  target_link_libraries(gammaray_kjobtracker_ui_plugin
    gammaray_ui
  )
endif()
