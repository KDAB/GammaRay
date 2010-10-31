project(endoscope)

set (QT_MIN_VERSION "4.7.0")

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories(
  ${QT_INCLUDES}
  ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTWEBKIT_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

# TODO find a nicer way for all this, ideally auto-detect the name mangling format, but at least guess a default based on OS + compiler
option(ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES FALSE "Enable if your compiler uses an unsupported C++ name mangling scheme")
if (ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
  add_definitions(-DENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
endif ()

set(endoscope_runner_srcs main.cpp)
add_executable(endoscope ${endoscope_runner_srcs})
target_link_libraries(endoscope ${QT_QTCORE_LIBRARY})

set(endoscope_srcs
  mainwindow.cpp
  probe.cpp
  propertywidget.cpp
  objectlistmodel.cpp
  objectpropertymodel.cpp
  objectdynamicpropertymodel.cpp
  objectstaticpropertymodel.cpp
  objectclassinfomodel.cpp
  objectmethodmodel.cpp
  objectenummodel.cpp
  objecttreemodel.cpp
  connectionmodel.cpp
  connectionfilterproxymodel.cpp
  scenemodel.cpp
  util.cpp
  widgetpreviewwidget.cpp
  methodinvocationdialog.cpp
  methodargumentmodel.cpp
  singlecolumnobjectproxymodel.cpp
  modeltester.cpp
  modelmodel.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
)
qt4_wrap_ui(endoscope_srcs
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
)
kde4_add_library(endoscope_probe SHARED ${endoscope_srcs})
target_link_libraries(endoscope_probe
  dl
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTSCRIPTTOOLS_LIBRARY}
  ${QT_QTWEBKIT_LIBRARY}
)
set_target_properties(endoscope_probe PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS endoscope endoscope_probe ${INSTALL_TARGETS_DEFAULT_ARGS})
