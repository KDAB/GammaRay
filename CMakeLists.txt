# This is the top-level CMakeLists.txt file for the GammaRay project.
#
# To build the man page from POD, run 'make man' after CMake (assumes perl is available)
# To install the resulting man page, run 'make install'
# Not available on Windows.
#
# To build the apidox, run 'make docs' after CMake (assumes doxygen is available)
#

project(GammaRay)

if(WIN32)
  # needed for automoc and QtMain to work correctly
  cmake_minimum_required(VERSION 2.8.11)
  cmake_policy(SET CMP0020 NEW)
else()
  cmake_minimum_required(VERSION 2.8.9)
endif()

if(NOT Prog_NAME)
  set(Prog_NAME "GammaRay")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(POLICY CMP0022)
  cmake_policy(SET CMP0022 OLD) # until we require 2.8.12
endif()

set(GAMMARAY_VERSION_MAJOR "1")
set(GAMMARAY_VERSION_MINOR "9")
set(GAMMARAY_VERSION_PATCH "96")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")
set(GAMMARAY_SOVERSION "2.0.0")
set(GAMMARAY_PLUGIN_VERSION "2.0")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

enable_testing()

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE _git_revision
    )
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

message(STATUS "Building ${Prog_NAME} ${GAMMARAY_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")
add_definitions(-DPROGRAM_NAME=\"${Prog_NAME}\")
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include(CheckCXXCompilerFlag)
include(CheckLibraryExists)
include(GammaRayMacros)
include(GammaRayMacrosInternal)
include(MacroLogFeature)

set(QT_MIN_VERSION "4.7.0")

option(
  GAMMARAY_ENFORCE_QT4_BUILD
  "Enable if you want to enfore a build with Qt4"
  OFF
)

option(
  GAMMARAY_PROBE_ONLY_BUILD
  "Build only the probe, not the host tools (client, launcher, etc)."
  OFF
)

# try Qt5 first, and prefer that (if found), but only if not disabled via option
if(NOT GAMMARAY_ENFORCE_QT4_BUILD)
  find_package(Qt5Core QUIET)
endif()

if(Qt5Core_FOUND)
  find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Gui Network)
  find_package(Qt5 NO_MODULE QUIET COMPONENTS
    Concurrent
    Test
    Widgets
    PrintSupport
    Svg
    Script
    ScriptTools
    Location # workaround for 5.1 webkit that tries to find a qtlocation with a matching version if none (with any version) has been found before...
    Designer
  )
  find_package(Qt5WebKitWidgets 5.0.2 QUIET) # 5.0.[01] has a code generation bug in uic that fails to generate proper code for webkitwidget classes

  include("cmake/ECMQt4To5Porting.cmake")
  # only needed for Qt <= 5.1 and/or some older CMake versions (exact combination unknown unfortunately)
  include_directories(${QT_INCLUDES})

  set(HAVE_QT_CONCURRENT ${Qt5Concurrent_FOUND})
  set(HAVE_QT_WIDGETS ${Qt5Widgets_FOUND})
  set(HAVE_QT_SVG ${Qt5Svg_FOUND})
  set(HAVE_QT_DESIGNER ${Qt5Designer_FOUND})
  set(HAVE_QT_PRINTSUPPORT ${Qt5PrintSupport_FOUND})
  set(HAVE_QT_WEBKIT1 ${Qt5WebKitWidgets_FOUND})

  if(Qt5_POSITION_INDEPENDENT_CODE AND NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

# Qt4
else()
  set(QT_USE_IMPORTED_TARGETS true)
  find_package(Qt4 ${QT_MIN_VERSION} REQUIRED QtCore QtGui QtNetwork)
  find_package(Qt4 ${QT_MIN_VERSION} QUIET COMPONENTS QtScript QtScriptTools QtWebKit QtDesigner QtSvg QtTest)

  include(${QT_USE_FILE})
  set(HAVE_QT_CONCURRENT true)
  set(HAVE_QT_WIDGETS true)
  set(HAVE_QT_SVG true)
  if(QT_QTDESIGNER_FOUND)
    set(HAVE_QT_DESIGNER true)
  endif()
  set(HAVE_QT_PRINTSUPPORT true)
  set(HAVE_QT_WEBKIT1 ${QT_QTWEBKIT_FOUND})
endif()

# TODO: Remove me once fixed in ECM module
if(Qt5Core_FOUND)
  # more hacks: find qpa/... includes
  # also see https://codereview.qt-project.org/#change,30483
  include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

  # TODO warnings rather than build errors for the deprecated methods would be nice...
  add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
endif()

# disable QT_STRICT_ITERATORS on the Qt5+Windows combo
# see: https://bugreports.qt-project.org/browse/QTBUG-29608
if(NOT (Qt5Core_FOUND AND WIN32))
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

set(BIN_INSTALL_DIR "bin")
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
set(INCLUDE_INSTALL_DIR "include/gammaray")
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/GammaRay)
if(WIN32)
  set(PLUGIN_INSTALL_DIR "plugins")
  set(LIBEXEC_INSTALL_DIR "${BIN_INSTALL_DIR}")
else()
  set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray")
  set(LIBEXEC_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray/libexec")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

# set RPATH only when installing to a non-default location
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if(${_isSystemPlatformLibDir} EQUAL -1 AND ${_isSystemCLibDir} EQUAL -1 AND ${_isSystemCxxLibDir} EQUAL -1)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
endif()

set(
  INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

# "inverse" install dirs, to find the base location again
gammaray_inverse_dir(GAMMARAY_INVERSE_BIN_DIR "${BIN_INSTALL_DIR}")
gammaray_inverse_dir(GAMMARAY_INVERSE_PROBE_DIR "${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/GAMMARAY_PROBE_ABI_DUMMY")
gammaray_inverse_dir(GAMMARAY_INVERSE_LIBEXEC_DIR "${LIBEXEC_INSTALL_DIR}")

macro_log_feature(
  HAVE_QT_CONCURRENT
  "QtConcurrent"
  "Qt concurrency library"
  "included with Qt"
  FALSE
  ""
  "Required for the GammaRay launcher process list"
)

macro_log_feature(
  HAVE_QT_WIDGETS
  "Qt Widgets"
  "Qt Widgets"
  "included with Qt"
  FALSE
  ""
  "Required for the GammaRay client UI and widget-related tools"
)

macro_log_feature(
  QT_QTSCRIPTTOOLS_FOUND
  "Script engine debugger in Qt"
  "A debugger for QtScript"
  "included with Qt"
  FALSE
  ""
  "Required for the script engine debugger tool"
)

macro_log_feature(
  HAVE_QT_WEBKIT1
  "WebKit in Qt"
  "A Qt-based web browser engine"
  "included with Qt"
  FALSE
  ""
  "Required for the webinspector tool"
)

macro_log_feature(
  HAVE_QT_SVG
  "Qt SVG"
  "Qt SVG renderer and generator"
  "http://qt-project.org/"
  FALSE
  ""
  "Required for widget SVG export"
)

macro_log_feature(
  HAVE_QT_DESIGNER
  "Qt Designer Library"
  "Qt graphical widget editor"
  "http://qt-project.org/"
  FALSE
  ""
  "Required for widget .ui file export"
)

macro_log_feature(
  HAVE_QT_PRINTSUPPORT
  "Qt Print Support"
  "Qt PDF generation"
  "http://qt-project.org/"
  FALSE
  ""
  "Required for widget PDF export"
)

if(NOT Qt5Core_FOUND)
find_path(
  QT_PRIVATE_INCLUDE_DIR private/qobject_p.h
  PATHS ${QT_INCLUDES}
)
if(QT_PRIVATE_INCLUDE_DIR)
  # not enough, some of them include harfbuzz headers, so we need to find those as well
  # for now we assume a regular Qt4 source build layout, but that probably should be generalized
  find_path(
    HARFBUZZ_INCLUDE_DIR harfbuzz.h
    PATH ${QT_PRIVATE_INCLUDE_DIR}/../../src/3rdparty/harfbuzz/src
  )
endif()

if(QT_PRIVATE_INCLUDE_DIR AND HARFBUZZ_INCLUDE_DIR)
  set(HAVE_PRIVATE_QT_HEADERS TRUE)
  include_directories(${HARFBUZZ_INCLUDE_DIR})
else()
  set(HAVE_PRIVATE_QT_HEADERS FALSE)
  # needs to go before Qt includes, in case we have non-working headers with the same name there
  include_directories(BEFORE ${CMAKE_SOURCE_DIR}/3rdparty/qt)
endif()
else() # Qt 5
  find_path(QT_PRIVATE_INCLUDE_DIR private/qobject_p.h PATHS ${Qt5Core_PRIVATE_INCLUDE_DIRS})
  if(QT_PRIVATE_INCLUDE_DIR)
    set(HAVE_PRIVATE_QT_HEADERS true)
  endif()
endif()

macro_log_feature(
  HAVE_PRIVATE_QT_HEADERS
  "Qt internals"
  "Private Qt headers, necessary for painter debugging/profiling."
  "http://developer.qt.nokia.com/"
  FALSE
  ${QT_MIN_VERSION}
  "You must have a build version of Qt available. Make sure the qmake found first in your execute comes from this build version."
)

if(WIN32 OR APPLE)
  set(BUILD_TIMER_PLUGIN TRUE)
else()
  check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
  macro_log_feature(HAVE_CLOCK_GETTIME "librt" "High resolution clock for the timer profiler plugin." "part of glibc" FALSE)
  set(BUILD_TIMER_PLUGIN ${HAVE_CLOCK_GETTIME})
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

include(CheckIncludeFiles)
check_include_files(stdint.h HAVE_STDINT_H)

include(GammaRayProbeABI) # this needs to be run after we know what exactly we are building

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray-version.h
)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GCC_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GCC_LOGICAL_OP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wcast-qual -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GCC_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GCC_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(MINGW)
    # mingw will error out on the crazy casts in probe.cpp without this
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  else()
    # visibility attributes not supported on mingw, don't use -fvisibility option
    # see: http://stackoverflow.com/questions/7994415/mingw-fvisibility-hidden-does-not-seem-to-work
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
  endif()
endif()

# linker flags
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

if(NOT CMAKE_DISABLE_FIND_PACKAGE_GRAPHVIZ)
  set(GRAPHVIZ_MIN_VERSION "2.20")
  find_package(Graphviz)
  macro_log_feature(
    GRAPHVIZ_FOUND
    "Graphviz"
    "Graph visualization software"
    "http://www.graphviz.org/"
    FALSE
    ${GRAPHVIZ_MIN_VERSION}
    "Needed for the state machine visualizer plugin"
  )
else()
  set(GRAPHVIZ_FOUND False)
endif()
set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
if(GRAPHVIZ_FOUND)
  add_definitions(-DGRAPHVIZ_MAJOR_VERSION=${GRAPHVIZ_MAJOR_VERSION} -DGRAPHVIZ_MINOR_VERSION=${GRAPHVIZ_MINOR_VERSION})
endif()

if(NOT CMAKE_DISABLE_FIND_PACKAGE_VTK)
  #VTK discovery works a lot better if you give CMake a hint using the VTK_DIR variable
  find_path(VTK_DIR VTKConfig.cmake
    /usr/lib64/vtk /usr/lib/vtk /usr/local/lib64/vtk /usr/local/lib/vtk
  )
  find_package(VTK)
  set(VTK_MESSAGE "Needed for the object visualizer plugin")
  if(VTK_FOUND)
    find_path(VTK_QT_INCLUDE_DIR NAMES QVTKWidget.h HINTS ${VTK_INCLUDE_DIRS})
    if(NOT VTK_QT_INCLUDE_DIR)
      set(VTK_FOUND FALSE)
      set(VTK_MESSAGE "Looks like VTK was not built with Qt (QVTKWidget is missing). Object visualizer plugin will not be built")
    endif()
  endif()
  macro_log_feature(
    VTK_FOUND
    "VTK"
    "Visualization Toolkit"
    "http://www.vtk.org"
    FALSE
    ""
    ${VTK_MESSAGE}
  )
else()
  set(VTK_FOUND False)
endif()
set(HAVE_VTK ${VTK_FOUND})

if(UNIX AND NOT APPLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  set(XDG_APPS_INSTALL_DIR share/applications)
  install(FILES GammaRay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

  install(
    FILES resources/GammaRay-16x16.png
    DESTINATION share/icons/hicolor/16x16/apps RENAME GammaRay.png
  )
  install(
    FILES resources/GammaRay-32x32.png
    DESTINATION share/icons/hicolor/32x32/apps RENAME GammaRay.png
  )
  install(
    FILES resources/GammaRay-48x48.png
    DESTINATION share/icons/hicolor/48x48/apps RENAME GammaRay.png
  )
  install(
    FILES resources/GammaRay-128x128.png
    DESTINATION share/icons/hicolor/128x128/apps RENAME GammaRay.png
  )
  install(
    FILES resources/GammaRay-256x256.png
    DESTINATION share/icons/hicolor/256x256/apps RENAME GammaRay.png
  )
  install(
    FILES resources/GammaRay-512x512.png
    DESTINATION share/icons/hicolor/512x512/apps RENAME GammaRay.png
  )
endif()

set(LICENSE_FILE "License.txt")
set(README_FILE "ReadMe.txt")
if(NOT APPLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
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

find_program(POD2MAN_EXECUTABLE pod2man)

if(POD2MAN_EXECUTABLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  #man page generation using pod2man
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/gammaray.1
    COMMAND ${POD2MAN_EXECUTABLE} -c "KDAB Products" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_SOURCE_DIR}/gammaray.pod ${CMAKE_BINARY_DIR}/gammaray.1
    DEPENDS ${CMAKE_SOURCE_DIR}/gammaray.pod
  )
  add_custom_target(man ALL DEPENDS ${CMAKE_BINARY_DIR}/gammaray.1)

  install(FILES ${CMAKE_BINARY_DIR}/gammaray.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray.h
)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}
)

include(ExternalProject)

add_subdirectory(cmake)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(hooking)
if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  add_subdirectory(launcher)
endif()
if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_subdirectory(ui)
  add_subdirectory(inprocessui)
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_subdirectory(client)
  endif()
endif()
if(Qt5Test_FOUND OR QT_QTTEST_FOUND)
  add_subdirectory(tests)
endif()
add_subdirectory(plugins)

find_package(Doxygen)
macro_log_feature(
  DOXYGEN_FOUND
  "Doxygen"
  "Documentation system"
  "http://www.doxygen.org"
  FALSE
  ""
  "Needed to build the optional API documention"
)
if(DOXYGEN_FOUND)
  file(GLOB _dox_deps docs/*.dox docs/*.html)
  gammaray_all_installed_headers(_all_hdrs)
  list(APPEND _all_hdrs ${_dox_deps})
  gammaray_join_list(DOXYGEN_INPUT " " ${_all_hdrs})

  #apidox generation using doxygen
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/apidocs/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
    DEPENDS ${_all_hdrs} ${_dox_deps} ${CMAKE_SOURCE_DIR}/Doxyfile.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
  )

endif()

macro_display_feature_log()
if(CMAKE_DISABLE_FIND_PACKAGE_VTK)
  message(STATUS "Skipped VTK discovery due to the CMAKE_DISABLE_FIND_PACKAGE_VTK option. Object visualizer plugin will not be built")
endif()
if(CMAKE_DISABLE_FIND_PACKAGE_GRAPHVIZ)
  message(STATUS "Skipped Graphviz discovery due to the CMAKE_DISABLE_FIND_PACKAGE_GRAPHVIZ option. State machine visualizer plugin will not be built")
endif()


#
# CMake package config file generation
#
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/GammaRayConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfig.cmake
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS INCLUDE_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfigVersion.cmake
                                 VERSION ${GAMMARAY_VERSION}
                                 COMPATIBILITY SameMajorVersion)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfigVersion.cmake
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(
  EXPORT GammaRayTargets
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  FILE GammaRayTarget.cmake
#     NAMESPACE GammaRay::
)
