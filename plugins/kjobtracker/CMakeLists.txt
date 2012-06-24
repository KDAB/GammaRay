find_package(KDE4)

macro_log_feature(
  KDE4_FOUND
  "KDE4"
  "KDE libraries"
  "http://www.kde.org/"
  FALSE
  ""
  "Required for the KJob tracker plug-in"
)


if (KDE4_FOUND)

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
  ${CMAKE_SOURCE_DIR}/3rdparty/kde/kfilterproxysearchline.cpp
)

qt4_automoc(${gammaray_kjob_plugin_srcs})

qt4_wrap_ui(gammaray_kjob_plugin_srcs
   kjobtracker.ui
)

gammaray_add_plugin(gammaray_kjobtracker_plugin
    gammaray_kjobtracker.desktop
    ${gammaray_kjob_plugin_srcs}
)

target_link_libraries(gammaray_kjobtracker_plugin
    ${KDE4_KDECORE_LIBS}
    gammaray_probe
)

endif()
