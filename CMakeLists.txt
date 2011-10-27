# This is the top-level CMakeLists.txt file for the GammaRay project.
#
# Pass the following variables to cmake to control the build:
#
# -DGAMMARAY_UNKNOWN_CXX_MANGLED_NAMES=[on|off]
#  Set this if your compiler uses an unsupported C++ name mangling scheme
#  Default=off
#
# To build the man page from POD, run 'make man' after CMake.
# To install the resulting man page, run 'make install'
#

project(GammaRay)
cmake_minimum_required(VERSION 2.8)

if(NOT Prog_NAME)
  set(Prog_NAME "GammaRay")
endif()

set(GAMMARAY_VERSION_MAJOR "1")
set(GAMMARAY_VERSION_MINOR "0")
set(GAMMARAY_VERSION_PATCH "1")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")

enable_testing()

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE _git_revision)
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

message(STATUS "Building ${Prog_NAME} ${GAMMARAY_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")
add_definitions(-DPROGRAM_NAME=\"${Prog_NAME}\")

include(CheckCXXCompilerFlag)

set(QT_MIN_VERSION "4.7.0")
find_package(Qt4 REQUIRED)

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

include_directories(
  ${QT_INCLUDES}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/qt4/plugins")

set(
  INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
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
  set(DOC_INSTALL_DIR share/doc/gammaray/)
else()
  set(DOC_INSTALL_DIR .)
endif()

# TODO: find a nicer way for all this. ideally auto-detect the name mangling
# format, but at least guess a default based on OS + compiler.
option(
  GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
  "Enable if your compiler uses an unsupported C++ name mangling scheme"
  OFF
)
if(GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES)
  add_definitions(-DGAMMARAY_UNKNOWN_CXX_MANGLED_NAMES)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray.h
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray-version.h
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

set(gammaray_runner_srcs
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
  set(gammaray_runner_srcs
    ${gammaray_runner_srcs}
    injector/gdbinjector.cpp
    attach/processlist_unix.cpp
  )
else()
  set(gammaray_runner_srcs
    ${gammaray_runner_srcs}
    attach/processlist_win.cpp
  )
endif()

qt4_wrap_ui(gammaray_runner_srcs attach/attachdialog.ui)
qt4_add_resources(gammaray_runner_srcs resources/gammaray.qrc)

add_executable(gammaray ${gammaray_runner_srcs})
target_link_libraries(gammaray ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set(gammaray_srcs
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
  multisignalmapper.cpp
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
  tools/textdocumentinspector/textdocumentcontentview.cpp
  tools/messagehandler/messagehandler.cpp
  tools/messagehandler/messagemodel.cpp
  toolmodel.cpp
  promolabel.cpp
  sidepane.cpp

  tools/codecbrowser/codecbrowser.cpp
  tools/connectioninspector/connectioninspector.cpp
  tools/fontbrowser/fontbrowser.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/modelinspector/modelinspector.cpp
  tools/modelinspector/modelinspectorwidget.cpp
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/sceneinspector/sceneinspector.cpp
  tools/scriptenginedebugger/scriptenginedebugger.cpp
  tools/selectionmodelinspector/selectionmodelinspector.cpp
  tools/statemachineinspector/statemachineinspector.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/webinspector/webinspector.cpp
  tools/widgetinspector/widgetinspector.cpp
  tools/widgetinspector/overlaywidget.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
  3rdparty/qt/resourcemodel.cpp
)

if(NOT WIN32)
  set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/backtrace_unix.cpp)
else()
  set(gammaray_srcs ${gammaray_srcs}
                    tools/messagehandler/backtrace_win.cpp
                    tools/messagehandler/StackWalker.cpp)
endif()

qt4_automoc(${gammaray_srcs} ${gammaray_runner_srcs})
qt4_wrap_ui(gammaray_srcs
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
  tools/sceneinspector/graphicssceneview.ui

  tools/codecbrowser/codecbrowser.ui
  tools/connectioninspector/connectioninspector.ui
  tools/fontbrowser/fontbrowser.ui
  tools/messagehandler/messagehandler.ui
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
qt4_add_resources(gammaray_srcs resources/gammaray.qrc)
add_library(gammaray_probe SHARED ${gammaray_srcs})
target_link_libraries(gammaray_probe
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSCRIPT_LIBRARY}
  ${QT_QTSCRIPTTOOLS_LIBRARY}
  ${QT_QTWEBKIT_LIBRARY}
)
if(NOT WIN32)
  target_link_libraries(gammaray_probe dl)
endif()
set_target_properties(gammaray_probe PROPERTIES PREFIX "")

install(TARGETS gammaray gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})

if(UNIX AND NOT APPLE)
  set(XDG_APPS_INSTALL_DIR share/applications)
  install(FILES GammaRay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

  install(FILES resources/GammaRay-16x16.png DESTINATION share/icons/hicolor/16x16/apps)
  install(FILES resources/GammaRay-32x32.png DESTINATION share/icons/hicolor/32x32/apps)
  install(FILES resources/GammaRay-48x48.png DESTINATION share/icons/hicolor/48x48/apps)
  install(FILES resources/GammaRay-128x128.png DESTINATION share/icons/hicolor/128x128/apps)
  install(FILES resources/GammaRay-256x256.png DESTINATION share/icons/hicolor/256x256/apps)
  install(FILES resources/GammaRay-512x512.png DESTINATION share/icons/hicolor/512x512/apps)

  install(FILES resources/kdablogo160.png resources/kdabproducts.png resources/splashscreen.png DESTINATION share/icons/hicolor/apps)
endif()

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
  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/gammaray.1
                     COMMAND pod2man -c "KDAB Products" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_SOURCE_DIR}/gammaray.pod ${CMAKE_BINARY_DIR}/gammaray.1
                     DEPENDS ${CMAKE_SOURCE_DIR}/gammaray.pod
                     )

  add_custom_target(man ALL DEPENDS ${CMAKE_BINARY_DIR}/gammaray.1)

  install(FILES ${CMAKE_BINARY_DIR}/gammaray.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)
endif()

include(ExternalProject)

add_subdirectory(tests)
