# This is the top-level CMakeLists.txt file for the Endoscope project.
#
# Pass the following variables to cmake to control the build:
#
# -DUSE_DETOURS=[on|off]
#  (Windows-only) Set to use detours if debugging
#  Default=off
#
# -DENDOSCOPE_UNKNOWN_CXX_MANGLED_NAMES=[on|off]
#  Set this if your compiler uses an unsupported C++ name mangling scheme
#  Default=off
#
# To build the man page from POD, run 'make man' after CMake.
# To install the resulting man page, run 'make install'
#

project(endoscope)
cmake_minimum_required(VERSION 2.8)

if(NOT Prog_NAME)
  set(Prog_NAME "Endoscope")
endif()

set(ENDOSCOPE_VERSION_MAJOR "1")
set(ENDOSCOPE_VERSION_MINOR "0")
set(ENDOSCOPE_VERSION_PATCH "0")
set(ENDOSCOPE_VERSION "${ENDOSCOPE_VERSION_MAJOR}.${ENDOSCOPE_VERSION_MINOR}.${ENDOSCOPE_VERSION_PATCH}")
set(ENDOSCOPE_VERSION_STRING "${ENDOSCOPE_VERSION}")

ENABLE_TESTING()

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE _git_revision)
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(ENDOSCOPE_VERSION_STRING "${ENDOSCOPE_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

if(APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
  set(CMAKE_INSTALL_PREFIX "/Applications")
endif()

message(STATUS "Building ${Prog_NAME} ${ENDOSCOPE_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")
add_definitions(-DPROGRAM_NAME=\"${Prog_NAME}\")

include(CheckCXXCompilerFlag)

set(QT_MIN_VERSION "4.7.0")
find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)

if(WIN32)
  option(USE_DETOURS "use detours in debugging" OFF)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

include_directories(
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

if(USE_DETOURS)
  add_definitions(-DUSE_DETOURS)
endif()

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
endif()

if(APPLE)
  # on the Mac support an extra install directory for application bundles
  set(
    INSTALL_TARGETS_DEFAULT_ARGS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
    BUNDLE DESTINATION "/Applications/Qt4"
  )
endif()

if(UNIX AND NOT APPLE)
  set(DOC_INSTALL_DIR share/doc/endoscope/)
else()
  set(DOC_INSTALL_DIR .)
endif()

# TODO: find a nicer way for all this. ideally auto-detect the name mangling
# format, but at least guess a default based on OS + compiler.
option(
  ENDOSCOPE_UNKNOWN_CXX_MANGLED_NAMES
  "Enable if your compiler uses an unsupported C++ name mangling scheme"
  OFF
)
if(ENDOSCOPE_UNKNOWN_CXX_MANGLED_NAMES)
  add_definitions(-DENDOSCOPE_UNKNOWN_CXX_MANGLED_NAMES)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-endoscope.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-endoscope.h
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-endoscope-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-endoscope-version.h
)

if(CMAKE_COMPILER_IS_GNUCXX)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GCC_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GCC_LOGICAL_OP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GCC_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GCC_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
endif()

add_subdirectory(injector)

set(endoscope_runner_srcs
  injector/abstractinjector.cpp
  injector/injectorfactory.cpp
  injector/preloadinjector.cpp
  injector/styleinjector.cpp
  injector/windllinjector.cpp
  injector/interactiveprocess.cpp
  probefinder.cpp
  main.cpp
  promolabel.cpp
  splashscreen.cpp

  attach/attachdialog.cpp
  attach/processmodel.cpp
  attach/processfiltermodel.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
)
if(NOT WIN32)
  set(endoscope_runner_srcs
    ${endoscope_runner_srcs}
    injector/gdbinjector.cpp
    attach/processlist_unix.cpp
  )
else()
  set(endoscope_runner_srcs
    ${endoscope_runner_srcs}
    attach/processlist_win.cpp
  )
endif()
if(USE_DETOURS)
  set(endoscope_runner_srcs
    ${endoscope_runner_srcs}
    injector/detourinjector.cpp
  )
endif()

qt4_wrap_ui(endoscope_runner_srcs attach/attachdialog.ui)
qt4_add_resources(endoscope_runner_srcs resources/endoscope.qrc)

add_executable(endoscope ${endoscope_runner_srcs})
target_link_libraries(endoscope ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if(USE_DETOURS)
  target_link_libraries(endoscope detours)
endif()

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
  tools/sceneinspector/scenemodel.cpp
  tools/statemachineinspector/statemodel.cpp
  tools/statemachineinspector/transitionmodel.cpp
  util.cpp
  tools/widgetinspector/widgetpreviewwidget.cpp
  methodinvocationdialog.cpp
  methodargumentmodel.cpp
  singlecolumnobjectproxymodel.cpp
  tools/modelinspector/modeltester.cpp
  tools/modelinspector/modelmodel.cpp
  tools/modelinspector/modelcellmodel.cpp
  tools/sceneinspector/graphicsview.cpp
  tools/sceneinspector/graphicssceneview.cpp
  tools/metatypebrowser/metatypesmodel.cpp
  tools/fontbrowser/fontmodel.cpp
  tools/codecbrowser/codecmodel.cpp
  tools/textdocumentinspector/textdocumentmodel.cpp
  tools/textdocumentinspector/textdocumentformatmodel.cpp
  toolmodel.cpp
  promolabel.cpp

  tools/codecbrowser/codecbrowser.cpp
  tools/connectioninspector/connectioninspector.cpp
  tools/fontbrowser/fontbrowser.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/modelinspector/modelinspector.cpp
  tools/modelinspector/modelinspectorwidget.cpp
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/sceneinspector/sceneinspector.cpp
  tools/scriptenginedebugger/scriptenginedebugger.cpp
  tools/selectionmodelinspector/selectionmodelinspector.cpp
  tools/statemachineinspector/statemachineinspector.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/webinspector/webinspector.cpp
  tools/widgetinspector/widgetinspector.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
  3rdparty/qt/resourcemodel.cpp
)
qt4_automoc(${endoscope_srcs} ${endoscope_runner_srcs})
qt4_wrap_ui(endoscope_srcs
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
  tools/sceneinspector/graphicssceneview.ui

  tools/codecbrowser/codecbrowser.ui
  tools/connectioninspector/connectioninspector.ui
  tools/fontbrowser/fontbrowser.ui
  tools/metatypebrowser/metatypebrowser.ui
  tools/modelinspector/modelinspectorwidget.ui
  tools/objectinspector/objectinspector.ui
  tools/resourcebrowser/resourcebrowser.ui
  tools/sceneinspector/sceneinspector.ui
  tools/scriptenginedebugger/scriptenginedebugger.ui
  tools/selectionmodelinspector/selectionmodelinspector.ui
  tools/statemachineinspector/statemachineinspector.ui
  tools/textdocumentinspector/textdocumentinspector.ui
  tools/webinspector/webinspector.ui
  tools/widgetinspector/widgetinspector.ui
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
else()
  if(USE_DETOURS)
    target_link_libraries(endoscope_probe detoured detours)
  endif()
endif()
set_target_properties(endoscope_probe PROPERTIES PREFIX "")

install(TARGETS endoscope endoscope_probe ${INSTALL_TARGETS_DEFAULT_ARGS})

set(LICENSE_FILE "License.txt")
set(README_FILE "ReadMe.txt")
if(NOT APPLE)
  install(FILES "${LICENSE_FILE}" "${README_FILE}" DESTINATION ${DOC_INSTALL_DIR})
endif()

add_subdirectory(qmldebugcontrol)

find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
  set(_cppcheck_flags "-I${CMAKE_CURRENT_BINARY_DIR}")
  get_directory_property(_inc_dirs INCLUDE_DIRECTORIES)
  foreach(_current ${_inc_dirs})
    set(_cppcheck_flags ${_cppcheck_flags} "-I${_current}")
  endforeach()
  add_custom_target(cppcheck
    COMMAND ${CPPCHECK_EXECUTABLE} --enable=all -j 4 --quiet ${_cppcheck_flags} ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running the cppcheck static code checker"
  )
endif()

if(UNIX)
  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/endoscope.1
                     COMMAND pod2man -c "KDAB Products" -r "\"${ENDOSCOPE_VERSION_STRING}\"" -s 1 ${CMAKE_SOURCE_DIR}/endoscope.pod ${CMAKE_BINARY_DIR}/endoscope.1
                     DEPENDS ${CMAKE_SOURCE_DIR}/endoscope.pod
                     )

  add_custom_target(man ALL DEPENDS ${CMAKE_BINARY_DIR}/endoscope.1)

  install(FILES ${CMAKE_BINARY_DIR}/endoscope.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)
endif()

include(ExternalProject)

set(QTC_SOURCE_DIR "" CACHE STRING "Path to Qt Creator sources")
set(QTC_BUILD_DIR "" CACHE STRING "Path to Qt Creator build directory")
if(QTC_SOURCE_DIR AND QTC_BUILD_DIR)
  message(STATUS "building the Qt Creator Endoscope plugin")
  ExternalProject_add(qtc-plugin
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtc-plugin"
    CONFIGURE_COMMAND ${QT_QMAKE_EXECUTABLE} QTC_SOURCE_DIR="${QTC_SOURCE_DIR}" IDE_BUILD_TREE="${QTC_BUILD_DIR}" ${CMAKE_CURRENT_SOURCE_DIR}/qtc-plugin
    INSTALL_COMMAND ""
  )
else(QTC_SOURCE_DIR AND QTC_BUILD_DIR)
  message(STATUS "not building the Qt Creator Endoscope plugin, set QTC_SOURCE_DIR and QTC_BUILD_DIR if you want it")
endif(QTC_SOURCE_DIR AND QTC_BUILD_DIR)

add_subdirectory(tests)
