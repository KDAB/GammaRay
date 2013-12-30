if(NOT Qt5Core_FOUND)
  # ugly workaround for FindKDE4Internal messing with the install rpath (https://github.com/KDAB/GammaRay/issues/36)
  set(_old_CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH})
  find_package(KDE4)
  set(CMAKE_INSTALL_RPATH ${_old_CMAKE_INSTALL_RPATH})
endif()

if(KDE4_FOUND AND NOT Qt5Core_FOUND)
  set(_KDE4_USABLE TRUE)
else()
  set(_KDE4_USABLE FALSE)
endif()

macro_log_feature(
  _KDE4_USABLE
  "KDE4"
  "KDE libraries"
  "http://www.kde.org/"
  FALSE
  ""
  "Required for the KJob tracker plug-in"
)

# probe part
if(KDE4_FOUND AND NOT Qt5Core_FOUND)
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

  target_link_libraries(gammaray_kjobtracker_plugin
    ${KDE4_KDECORE_LIBS}
    gammaray_core
  )
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
