project(endoscope)

include_directories(${KDE4_INCLUDES})

set(endoscope_runner_srcs main.cpp)
kde4_add_executable(endoscope ${endoscope_runner_srcs})
target_link_libraries(endoscope ${KDE4_KDECORE_LIBRARY})

set(endoscope_srcs
  mainwindow.cpp
  probe.cpp
  propertywidget.cpp
  objectlistmodel.cpp
  objectpropertymodel.cpp
  objectdynamicpropertymodel.cpp
  objectstaticpropertymodel.cpp
  objecttreemodel.cpp
  scenemodel.cpp
)
kde4_add_ui_files(endoscope_srcs
  mainwindow.ui
  propertywidget.ui
)
kde4_add_library(endoscope_probe SHARED ${endoscope_srcs})
target_link_libraries(endoscope_probe
  dl
  ${KDE4_KDECORE_LIBRARY}
  ${KDE4_KDEUI_LIBRARY}
)
set_target_properties(endoscope_probe PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS endoscope endoscope_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
