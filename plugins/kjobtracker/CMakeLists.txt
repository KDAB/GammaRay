# ugly workaround for FindKDE4Internal messing with the install rpath (https://github.com/KDAB/GammaRay/issues/36)
set(_old_CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH})
find_package(KDE4)
set(CMAKE_INSTALL_RPATH ${_old_CMAKE_INSTALL_RPATH})

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

if(KDE4_FOUND AND NOT Qt5Core_FOUND)

include_directories(
  ${KDE4_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(
  ${KDE4_DEFINITIONS}
)

set(gammaray_kjob_plugin_srcs
  kjobmodel.cpp
  kjobtracker.cpp
  kjobtrackerwidget.cpp
)

qt4_wrap_ui(gammaray_kjob_plugin_srcs
  kjobtrackerwidget.ui
)

gammaray_add_plugin(gammaray_kjobtracker_plugin
  gammaray_kjobtracker.desktop
  ${gammaray_kjob_plugin_srcs}
)

target_link_libraries(gammaray_kjobtracker_plugin
  ${KDE4_KDECORE_LIBS}
  gammaray_core
  gammaray_ui
)

endif()
