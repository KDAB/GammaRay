# This is the top-level CMakeLists.txt file for the GammaRay project.
#
# Pass the following variables to cmake to control the build:
# (See Install.txt for more information)
#
# -DGAMMARAY_ENFORCE_QT4_BUILD=[true|false]
#  Force building against Qt4, even if Qt5 is found.
#  Default=false
#
# -DGAMMARAY_BUILD_UI=[true|false]
#  Build the client and in-process UI.
#  Default=true
#
# -DGAMMARAY_PROBE_ONLY_BUILD=[true|false]
#  Build only an additional probe configuration for an already existing launcher.
#  Default=false
#
# -DGAMMARAY_UNKNOWN_CXX_MANGLED_NAMES=[true|false]
#  Enable if your compiler uses an unsupported C++ name mangling scheme.
#  Default=false
#
# To build the man page from POD, run 'make man' after CMake (assumes perl is available)
# To install the resulting man page, run 'make install'
# Not available on Windows.
#
# To build the apidox, run 'make docs' after CMake (assumes doxygen is available)
#

project(GammaRay)

cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0020 NEW)
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

include(CheckCXXCompilerFlag)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(GammaRayMacros)
include(GammaRayMacrosInternal)
include(FeatureSummary)

# Exit for blacklisted compilers (those that don't support C++11 very well)
#  MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
#  Clang 3.0
set(BAD_CXX_MESSAGE "")
if(MSVC)
  if(MSVC_VERSION LESS 1500)
    set(BAD_CXX_MESSAGE "MSVC 2008 or higher")
  endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.1.0)
    set(BAD_CXX_MESSAGE "Clang v3.1.0 or higher")
  endif()
endif()
if(BAD_CXX_MESSAGE)
  message(FATAL_ERROR "\nSorry, ${BAD_CXX_MESSAGE} is required to build this software. Please retry using a modern compiler that supports C++11 lambdas.")
endif()

# Enable the test harness
enable_testing()

# Version setup
set(GAMMARAY_VERSION_MAJOR "2")
set(GAMMARAY_VERSION_MINOR "2")
set(GAMMARAY_VERSION_PATCH "1")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")
set(GAMMARAY_SOVERSION "2.2.1")
set(GAMMARAY_PLUGIN_VERSION "2.2")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  set_package_properties(Git PROPERTIES TYPE OPTIONAL PURPOSE "Determine exact build version.")
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

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray-version.h
)

message(STATUS "Building GammaRay ${GAMMARAY_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")

#
# Build options
#
option(
  GAMMARAY_ENFORCE_QT4_BUILD
  "Enable if you want to enforce a build with Qt4"
  OFF
)

option(
  GAMMARAY_BUILD_UI
  "Build the GammaRay client and in-process UI."
  ON
)

option(
  GAMMARAY_PROBE_ONLY_BUILD
  "Build only an additional probe configuration for an already existing launcher."
  OFF
)

# TODO: find a nicer way for all this. ideally auto-detect the name mangling
# format, but at least guess a default based on OS + compiler.
option(
  GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
  "Enable if your compiler uses an unsupported C++ name mangling scheme"
  OFF
)

#
# Compiler & linker settings
#
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-std=c++0x HAVE_GXX_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") # required in C++11 mode
  endif()
  if(HAVE_GXX_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
  if(HAVE_GXX_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GXX_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GXX_POINTER_MEMACCESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
  if(HAVE_GXX_REORDER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreorder")
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

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

# linker flags
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

#
# Finding Qt
#

set(QT_MIN_VERSION "4.8.0")

# try Qt5 first, and prefer that (if found), but only if not disabled via option
if(NOT GAMMARAY_ENFORCE_QT4_BUILD)
  find_package(Qt5Core 5.1 QUIET)
else()
  set(Qt5Core_FOUND FALSE)
endif()

if(Qt5Core_FOUND)
  set_package_properties(Qt5Core PROPERTIES TYPE REQUIRED)
  find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Gui Network)
  find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS
    Concurrent
    Designer
    PrintSupport
    Qml
    Quick
    Svg
    Script
    ScriptTools
    Test
    WebKitWidgets
    Widgets
  )
  if(GAMMARAY_BUILD_UI) # widgets are required for the UI
    find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Widgets)
  endif()

  #HACK: CMake with broken Qt5Qml_PRIVATE_INCLUDE_DIRS, Qt5Quick_PRIVATE_INCLUDE_DIRS
  if(${Qt5Qml_FOUND})
    if(NOT "${Qt5Qml_PRIVATE_INCLUDE_DIRS}" MATCHES "/QtQml/")
      string(REPLACE "/QtCore" "/QtQml" replaceme "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
      list(APPEND Qt5Qml_PRIVATE_INCLUDE_DIRS ${replaceme})
      list(REMOVE_DUPLICATES Qt5Qml_PRIVATE_INCLUDE_DIRS)
    endif()
  endif()
  if(${Qt5Quick_FOUND})
    if(NOT "${Qt5Quick_PRIVATE_INCLUDE_DIRS}" MATCHES "/QtQuick/")
      string(REPLACE "/QtCore" "/QtQuick" replaceme "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
      list(APPEND Qt5Quick_PRIVATE_INCLUDE_DIRS ${Qt5Qml_PRIVATE_INCLUDE_DIRS})
      list(APPEND Qt5Quick_PRIVATE_INCLUDE_DIRS ${replaceme})
      list(REMOVE_DUPLICATES Qt5Quick_PRIVATE_INCLUDE_DIRS)
    endif()
  endif()

  include("cmake/ECMQt4To5Porting.cmake")

  set(HAVE_QT_CONCURRENT ${Qt5Concurrent_FOUND})
  set(HAVE_QT_WIDGETS ${Qt5Widgets_FOUND})
  set(HAVE_QT_SVG ${Qt5Svg_FOUND})
  set(HAVE_QT_DESIGNER ${Qt5Designer_FOUND})
  set(HAVE_QT_PRINTSUPPORT ${Qt5PrintSupport_FOUND})
  set(HAVE_QT_WEBKIT1 ${Qt5WebKitWidgets_FOUND})

  if(Qt5_POSITION_INDEPENDENT_CODE AND NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

  # TODO: Remove me once fixed in ECM module
  # more hacks: find qpa/... includes
  # also see https://codereview.qt-project.org/#change,30483
  include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

  # TODO warnings rather than build errors for the deprecated methods would be nice...
  add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)

  find_path(QT_PRIVATE_INCLUDE_DIR private/qobject_p.h PATHS ${Qt5Core_PRIVATE_INCLUDE_DIRS})
  if(QT_PRIVATE_INCLUDE_DIR)
    set(HAVE_PRIVATE_QT_HEADERS true)
  endif()

  find_path(_Qt5Quick_PRIVATE_INCLUDE_DIR private/qquickitem_p.h PATHS ${Qt5Quick_PRIVATE_INCLUDE_DIRS})
  if(_Qt5Quick_PRIVATE_INCLUDE_DIR)
    set(HAVE_PRIVATE_Qt5Quick_HEADERS true)
  endif()

  set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/")
  set_package_properties(Qt5Concurrent PROPERTIES TYPE RECOMMENDED PURPOSE "Required for the GammaRay launcher process list.")
  set_package_properties(Qt5Widget PROPERTIES TYPE RECOMMENDED PURPOSE "Required for the GammaRay client UI and widget-related tools.")
  set_package_properties(Qt5Svg PROPERTIES TYPE OPTIONAL PURPOSE "Required for widget SVG export.")
  set_package_properties(Qt5PrintSupport PROPERTIES TYPE OPTIONAL PURPOSE "Required for widget PDF export.")
  add_feature_info("QPainter analyzer" HAVE_PRIVATE_QT_HEADERS "Requires private Qt headers to be available.")

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

  find_path(QT_PRIVATE_INCLUDE_DIR private/qobject_p.h PATHS ${QT_INCLUDES})
  if(QT_PRIVATE_INCLUDE_DIR)
    # not enough, some of them include harfbuzz headers, so we need to find those as well
    # for now we assume a regular Qt4 source build layout, but that probably should be generalized
    find_path(HARFBUZZ_INCLUDE_DIR harfbuzz.h PATH ${QT_PRIVATE_INCLUDE_DIR}/../../src/3rdparty/harfbuzz/src)
  endif()

  if(QT_PRIVATE_INCLUDE_DIR AND HARFBUZZ_INCLUDE_DIR)
    set(HAVE_PRIVATE_QT_HEADERS TRUE)
    include_directories(${HARFBUZZ_INCLUDE_DIR})
  else()
    set(HAVE_PRIVATE_QT_HEADERS FALSE)
    # needs to go before Qt includes, in case we have non-working headers with the same name there
    include_directories(BEFORE ${CMAKE_SOURCE_DIR}/3rdparty/qt)
  endif()

  set_package_properties(Qt4 PROPERTIES URL "http://qt-project.org/")
  add_feature_info("QPainter analyzer" HAVE_PRIVATE_QT_HEADERS
    "You must have a build version of Qt available. Make sure the qmake found first in your execute comes from this build version."
  )

endif()

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
# disable QT_STRICT_ITERATORS on the Qt5+Windows combo
# see: https://bugreports.qt-project.org/browse/QTBUG-29608
if(NOT (Qt5Core_FOUND AND WIN32))
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

add_feature_info("QtScript debugger" QT_QTSCRIPTTOOLS_FOUND "Requires QtScript and QtScriptTools.")
add_feature_info("Web inspector" HAVE_QT_WEBKIT1 "Requires QtWebKit.")
add_feature_info("Widget .ui file export" HAVE_QT_DESIGNER "Requires QtDesigner library.")

#
# Installation settings
#
set(BIN_INSTALL_DIR "bin")
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
set(INCLUDE_INSTALL_DIR "include/gammaray")
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/GammaRay)
set(XDG_APPS_INSTALL_DIR share/applications)
set(APPDATA_INSTALL_DIR share/appdata)
if(WIN32)
  set(PLUGIN_INSTALL_DIR "plugins")
  set(LIBEXEC_INSTALL_DIR "${BIN_INSTALL_DIR}")
else()
  set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray")
  set(LIBEXEC_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray/libexec")
endif()
if(UNIX AND NOT APPLE)
  set(DOC_INSTALL_DIR share/doc/gammaray/)
else()
  set(DOC_INSTALL_DIR .)
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
  BUNDLE DESTINATION "/Applications/Qt4"
)

# "inverse" install dirs, to find the base location again
gammaray_inverse_dir(GAMMARAY_INVERSE_BIN_DIR "${BIN_INSTALL_DIR}")
gammaray_inverse_dir(GAMMARAY_INVERSE_PROBE_DIR "${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/GAMMARAY_PROBE_ABI_DUMMY")
gammaray_inverse_dir(GAMMARAY_INVERSE_LIBEXEC_DIR "${LIBEXEC_INSTALL_DIR}")

#
# Additional dependencies
#

if(Qt5Core_FOUND AND Qt5Core_VERSION VERSION_LESS 5.2.2)
  # https://codereview.qt-project.org/75530
  message(STATUS "Disabling timer profiler plug-in due to a bug in Qt5 <= 5.2.1.")
  set(BUILD_TIMER_PLUGIN FALSE)
else()
  if(WIN32 OR APPLE)
    set(BUILD_TIMER_PLUGIN TRUE)
  else()
    check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)
    gammaray_add_dummy_package(rt HAVE_CLOCK_GETTIME)
    set_package_properties(rt PROPERTIES
      TYPE RECOMMENDED
      DESCRIPTION "High resolution clock, part of glibc"
      PURPOSE "Needed for the timer profiler plugin."
    )
    set(BUILD_TIMER_PLUGIN ${HAVE_CLOCK_GETTIME})
  endif()
endif()

check_include_files(stdint.h HAVE_STDINT_H)

set(GRAPHVIZ_MIN_VERSION "2.20")
find_package(Graphviz)
set_package_properties(Graphviz PROPERTIES
  TYPE RECOMMENDED
  DESCRIPTION "Graph visualization software"
  URL "http://www.graphviz.org/"
  PURPOSE "Needed for the state machine visualizer plugin."
)
set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
if(GRAPHVIZ_FOUND)
  add_definitions(-DGRAPHVIZ_MAJOR_VERSION=${GRAPHVIZ_MAJOR_VERSION} -DGRAPHVIZ_MINOR_VERSION=${GRAPHVIZ_MINOR_VERSION})
endif()

#VTK discovery works a lot better if you give CMake a hint using the VTK_DIR variable
find_path(VTK_DIR VTKConfig.cmake
  /usr/lib64/vtk /usr/lib/vtk /usr/local/lib64/vtk /usr/local/lib/vtk
)
find_package(VTK)
set_package_properties(VTK PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Visualization Toolkit"
  PURPOSE "Needed for the object visualizer plugin."
  URL "http://www.vtk.org"
)
if(VTK_FOUND)
  find_path(VTK_QT_INCLUDE_DIR NAMES QVTKWidget.h HINTS ${VTK_INCLUDE_DIRS})
  if(NOT VTK_QT_INCLUDE_DIR)
    message(STATUS "Cannot locate QVTKWidget.h in ${VTK_INCLUDE_DIRS}")
    set(VTK_FOUND FALSE)
    add_feature_info("QVTKWidget header" VTK_FOUND "Object visualizer plugin requires QVTKWidget.h header.")
  endif()
endif()
set(HAVE_VTK ${VTK_FOUND})

# ELF header for ABI detection
find_file(ELF_H elf.h)
if(ELF_H)
  set(HAVE_ELF_H TRUE)
endif()
add_feature_info("ELF ABI detection" HAVE_ELF_H "Automatic probe ABI detection on ELF-based systems. Requires elf.h.")

#
# actually build the stuff
#
include(GammaRayProbeABI) # this needs to be run after we know what exactly we are building

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray.h
)
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}
)

add_subdirectory(cmake)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(docs)
add_subdirectory(hooking)
if(NOT GAMMARAY_PROBE_ONLY_BUILD)
  add_subdirectory(launcher)
endif()
if(Qt5Widgets_FOUND OR QT_QTGUI_FOUND)
  add_subdirectory(ui)
  if(GAMMARAY_BUILD_UI)
    add_subdirectory(inprocessui)
  endif()
  if(GAMMARAY_BUILD_UI AND NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_subdirectory(client)
  endif()
endif()
if((Qt5Test_FOUND OR QT_QTTEST_FOUND) AND NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(tests)
endif()
add_subdirectory(plugins)
add_subdirectory(resources)

if(NOT APPLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  if(UNIX AND GAMMARAY_BUILD_UI)
    install(FILES GammaRay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES GammaRay.appdata.xml DESTINATION ${APPDATA_INSTALL_DIR})
  endif()
  install(FILES License.txt ReadMe.txt DESTINATION ${DOC_INSTALL_DIR})
endif()

#
# cppcheck
#
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

#
# CMake package config file generation
#
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/GammaRayConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfig.cmake
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/GammaRayConfigVersion.cmake
  VERSION ${GAMMARAY_VERSION}
  COMPATIBILITY SameMajorVersion
)

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

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
