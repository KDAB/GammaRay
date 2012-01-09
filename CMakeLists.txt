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

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})

set(GAMMARAY_VERSION_MAJOR "1")
set(GAMMARAY_VERSION_MINOR "1")
set(GAMMARAY_VERSION_PATCH "0")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

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
include(CheckLibraryExists)
include(GammaRayMacros)

find_package(Qt5Core QUIET)

if(Qt5Core_FOUND)

  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Designer REQUIRED)
  find_package(Qt5Script REQUIRED)
  find_package(Qt5Network REQUIRED)
  find_package(Qt5ScriptTools REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Svg REQUIRED)
  find_package(Qt5PrintSupport REQUIRED)
  find_package(Qt5Test REQUIRED)
  find_package(Qt5Xml REQUIRED)
  include(CMakeQt4To5Porting.cmake) # TODO: Port away from this.

else()
  set(QT_MIN_VERSION "4.7.0")
  find_package(Qt4 REQUIRED)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

include_directories(
  ${QT_INCLUDES}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_SOURCE_DIR}/3rdparty/kde
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

# TODO: Make this work on Windows, and test this on Mac
check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
if(HAVE_CLOCK_GETTIME)
  set(BUILD_TIMER_PLUGIN TRUE)
  message(STATUS "clock_gettime in librt found, building timer plugin.")
elseif(WIN32)
  set(BUILD_TIMER_PLUGIN TRUE)
  message(STATUS "windows, building timer plugin.")
else()
  set(BUILD_TIMER_PLUGIN FALSE)
  message(STATUS "clock_gettime in librt not found, disabling timer plugin.")
endif()

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

if(MINGW)
  # mingw will error out on the crazy casts in probe.cpp without this
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

# linker flags
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

add_subdirectory(injector)
add_subdirectory(launcher)

find_package(Graphviz)
find_package(VTK)

set(gammaray_srcs
  mainwindow.cpp
  metaobject.cpp
  metaobjectrepository.cpp
  metaproperty.cpp
  metapropertymodel.cpp
  pluginmanager.cpp
  probe.cpp

  propertywidget.cpp
  propertyeditor/propertycoloreditor.cpp
  propertyeditor/propertydoublepaireditor.cpp
  propertyeditor/propertyeditorfactory.cpp
  propertyeditor/propertyextendededitor.cpp
  propertyeditor/propertyfonteditor.cpp
  propertyeditor/propertyintpaireditor.cpp

  proxydetacher.cpp
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
  tools/localeinspector/localemodel.cpp
  tools/localeinspector/localedataaccessor.cpp
  tools/localeinspector/localeaccessormodel.cpp
  tools/codecbrowser/codecmodel.cpp
  tools/textdocumentinspector/textdocumentmodel.cpp
  tools/textdocumentinspector/textdocumentformatmodel.cpp
  tools/textdocumentinspector/textdocumentcontentview.cpp
  tools/messagehandler/messagehandler.cpp
  tools/messagehandler/messagemodel.cpp
  toolmodel.cpp
  proxydetacher.cpp
  proxytoolfactory.cpp
  sidepane.cpp

  tools/codecbrowser/codecbrowser.cpp
  tools/connectioninspector/connectioninspector.cpp
  tools/fontbrowser/fontbrowser.cpp
  tools/localeinspector/localeinspector.cpp
  tools/metatypebrowser/metatypebrowser.cpp
  tools/modelinspector/modelinspector.cpp
  tools/modelinspector/modelinspectorwidget.cpp
  tools/objectinspector/objectinspector.cpp
  tools/resourcebrowser/resourcebrowser.cpp
  tools/resourcebrowser/resourcefiltermodel.cpp
  tools/sceneinspector/sceneinspector.cpp
  tools/selectionmodelinspector/selectionmodelinspector.cpp
  tools/textdocumentinspector/textdocumentinspector.cpp
  tools/widgetinspector/widgetinspector.cpp
  tools/widgetinspector/overlaywidget.cpp
  tools/widgetinspector/widgettreemodel.cpp

  3rdparty/kde/kidentityproxymodel.cpp
  3rdparty/kde/krecursivefilterproxymodel.cpp
  3rdparty/kde/kfilterproxysearchline.cpp
  3rdparty/qt/resourcemodel.cpp

  hooking/abstractfunctionoverwriter.cpp
  hooking/functionoverwriterfactory.cpp
  hooking/winfunctionoverwriter.cpp
  hooking/unixfunctionoverwriter.cpp
)

if(BUILD_TIMER_PLUGIN)
  set(gammaray_srcs
    ${gammaray_srcs}
    tools/timertop/timertop.cpp
    tools/timertop/timermodel.cpp
    tools/timertop/timerinfo.cpp
    tools/timertop/functioncalltimer.cpp
  )
endif()

if(NOT WIN32)
  set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/backtrace_unix.cpp)
elseif(MINGW)
  set(gammaray_srcs ${gammaray_srcs} tools/messagehandler/backtrace_dummy.cpp)
else()
  set(gammaray_srcs ${gammaray_srcs}
                    tools/messagehandler/backtrace_win.cpp
                    tools/messagehandler/StackWalker.cpp)
endif()

qt4_automoc(${gammaray_srcs})
qt4_wrap_ui(gammaray_srcs
  mainwindow.ui
  propertywidget.ui
  methodinvocationdialog.ui
  propertyeditor/propertydoublepaireditor.ui
  propertyeditor/propertyextendededitor.ui
  propertyeditor/propertyintpaireditor.ui
  tools/sceneinspector/graphicssceneview.ui
  tools/codecbrowser/codecbrowser.ui
  tools/connectioninspector/connectioninspector.ui
  tools/fontbrowser/fontbrowser.ui
  tools/localeinspector/localeinspector.ui
  tools/messagehandler/messagehandler.ui
  tools/metatypebrowser/metatypebrowser.ui
  tools/modelinspector/modelinspectorwidget.ui
  tools/objectinspector/objectinspector.ui
  tools/resourcebrowser/resourcebrowser.ui
  tools/sceneinspector/sceneinspector.ui
  tools/selectionmodelinspector/selectionmodelinspector.ui
  tools/textdocumentinspector/textdocumentinspector.ui
  tools/widgetinspector/widgetinspector.ui
)

if(BUILD_TIMER_PLUGIN)
  qt4_wrap_ui(gammaray_srcs
    tools/timertop/timertop.ui
  )
endif()

qt4_add_resources(gammaray_srcs resources/gammaray.qrc)
add_library(gammaray_probe
  SHARED ${gammaray_srcs}
)
if(BUILD_TIMER_PLUGIN AND NOT WIN32)
  target_link_libraries(gammaray_probe rt)
endif()
target_link_libraries(gammaray_probe
  ${QT_QTCORE_LIBRARIES}
  ${QT_QTGUI_LIBRARIES}
)
if(NOT WIN32)
  target_link_libraries(gammaray_probe dl)
endif()
set_target_properties(gammaray_probe PROPERTIES PREFIX "")

install(TARGETS gammaray_probe ${INSTALL_TARGETS_DEFAULT_ARGS})

if(UNIX AND NOT APPLE)
  set(XDG_APPS_INSTALL_DIR share/applications)
  install(FILES GammaRay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

  install(FILES resources/GammaRay-16x16.png DESTINATION share/icons/hicolor/16x16/apps RENAME GammaRay.png)
  install(FILES resources/GammaRay-32x32.png DESTINATION share/icons/hicolor/32x32/apps RENAME GammaRay.png)
  install(FILES resources/GammaRay-48x48.png DESTINATION share/icons/hicolor/48x48/apps RENAME GammaRay.png)
  install(FILES resources/GammaRay-128x128.png DESTINATION share/icons/hicolor/128x128/apps RENAME GammaRay.png)
  install(FILES resources/GammaRay-256x256.png DESTINATION share/icons/hicolor/256x256/apps RENAME GammaRay.png)
  install(FILES resources/GammaRay-512x512.png DESTINATION share/icons/hicolor/512x512/apps RENAME GammaRay.png)
endif()

set(LICENSE_FILE "License.txt")
set(README_FILE "ReadMe.txt")
if(NOT APPLE)
  install(FILES "${LICENSE_FILE}" "${README_FILE}" DESTINATION ${DOC_INSTALL_DIR})
endif()

find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
  set(_cppcheck_flags "-I${CMAKE_CURRENT_BINARY_DIR}")
  get_directory_property(_inc_dirs INCLUDE_DIRECTORIES)
  foreach(_current ${_inc_dirs})
    set(_cppcheck_flags ${_cppcheck_flags} "-I${_current}")
  endforeach()
  get_directory_property(_defs COMPILE_DEFINITIONS)
  foreach(_current ${_defs})
    set(_cppcheck_flags ${_cppcheck_flags} "-D${_current}")
  endforeach()

  add_custom_target(cppcheck
    COMMAND ${CPPCHECK_EXECUTABLE} --enable=all -j 4 --suppress=*:${QT_INCLUDE_DIR}* ${_cppcheck_flags}
      -i${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
      -i${CMAKE_CURRENT_SOURCE_DIR}/tests
      ${CMAKE_CURRENT_SOURCE_DIR}
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
add_subdirectory(tools)
