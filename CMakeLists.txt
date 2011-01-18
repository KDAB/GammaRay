project(endoscope)
cmake_minimum_required(VERSION 2.6)

# library versioning
set(GENERIC_LIB_VERSION "1.0.0")
set(GENERIC_LIB_SOVERSION "1")

set(QT_MIN_VERSION "4.6.0")
find_package(Qt4 REQUIRED)

include_directories(
  ${QT_INCLUDES}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}/3rdparty
)

set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION bin
                                 LIBRARY DESTINATION lib
                                 ARCHIVE DESTINATION lib COMPONENT Devel)
if(APPLE)
  # on the Mac support an extra install directory for application bundles
  set(INSTALL_TARGETS_DEFAULT_ARGS ${INSTALL_TARGETS_DEFAULT_ARGS}
                                     BUNDLE DESTINATION "/Applications/Qt4")
endif(APPLE)


# TODO find a nicer way for all this, ideally auto-detect the name mangling format, but at least guess a default based on OS + compiler
option(ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES FALSE "Enable if your compiler uses an unsupported C++ name mangling scheme")
if(ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
  add_definitions(-DENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
endif()

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
  statemodel.cpp
  util.cpp
  widgetpreviewwidget.cpp
  methodinvocationdialog.cpp
  methodargumentmodel.cpp
  singlecolumnobjectproxymodel.cpp
  modeltester.cpp
  modelmodel.cpp
  modelcellmodel.cpp
  graphicsview.cpp
  graphicssceneview.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
  3rdparty/qt/resourcemodel.cpp
)
qt4_automoc(${endoscope_srcs})
qt4_wrap_ui(endoscope_srcs
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
  graphicssceneview.ui
)
add_library(endoscope_probe SHARED ${endoscope_srcs})
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
