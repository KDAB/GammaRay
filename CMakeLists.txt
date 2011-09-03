project(endoscope)
cmake_minimum_required(VERSION 2.6)

set(QT_MIN_VERSION "4.6.0")
find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)

if(WIN32)
  option(USE_DETOURS "use detours for in debugging" OFF)
endif(WIN32)

include_directories(
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}/3rdparty
)

if(USE_DETOURS)
  add_definitions(-DUSE_DETOURS)
endif(USE_DETOURS)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

set(
  INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

if(WIN32)
  add_definitions(-DUNICODE)
endif(WIN32)

if(APPLE)
  # on the Mac support an extra install directory for application bundles
  set(
    INSTALL_TARGETS_DEFAULT_ARGS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
    BUNDLE DESTINATION "/Applications/Qt4"
  )
endif(APPLE)


# TODO: find a nicer way for all this. ideally auto-detect the name mangling
# format, but at least guess a default based on OS + compiler.
option(
  ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES
  "Enable if your compiler uses an unsupported C++ name mangling scheme"
  OFF
)
if(ENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
  add_definitions(-DENDOSCOPE_UKNOWN_CXX_MANGLED_NAMES)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-endoscope.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-endoscope.h
)

add_subdirectory(injector)

set(endoscope_runner_srcs
  injector/abstractinjector.cpp
  injector/detourinjector.cpp
  injector/gdbinjector.cpp
  injector/injectorfactory.cpp
  injector/preloadinjector.cpp
  injector/styleinjector.cpp
  injector/windllinjector.cpp
  probefinder.cpp
  main.cpp
)

add_executable(endoscope ${endoscope_runner_srcs})
target_link_libraries(endoscope ${QT_QTCORE_LIBRARY})
if(USE_DETOURS)
  target_link_libraries(endoscope detours)
endif(USE_DETOURS)

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
  transitionmodel.cpp
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
  metatypesmodel.cpp
  fontmodel.cpp
  codecmodel.cpp
  tools/textdocumentinspector/textdocumentmodel.cpp
  tools/textdocumentinspector/textdocumentformatmodel.cpp
  toolmodel.cpp

  tools/scriptenginedebugger/scriptenginedebugger.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/webinspector/webinspector.cpp

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

  tools/scriptenginedebugger/scriptenginedebugger.ui
  tools/textdocumentinspector/textdocumentinspector.ui
  tools/webinspector/webinspector.ui
)
qt4_add_resources(endoscope_srcs resources/endoscope.qrc)
add_library(endoscope_probe SHARED ${endoscope_srcs})
target_link_libraries(endoscope_probe
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTSCRIPTTOOLS_LIBRARY}
  ${QT_QTWEBKIT_LIBRARY}
)
if(NOT WIN32)
  target_link_libraries(endoscope_probe dl)
else(NOT WIN32)
  if(USE_DETOURS)
    target_link_libraries(endoscope_probe detoured detours)
  endif(USE_DETOURS)
endif(NOT WIN32)
set_target_properties(endoscope_probe PROPERTIES PREFIX "")

install(TARGETS endoscope endoscope_probe ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(qmldebugcontrol)
