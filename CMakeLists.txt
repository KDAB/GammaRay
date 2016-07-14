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
# -DGAMMARAY_INSTALL_QT_LAYOUT=[true|false]
#  Install GammaRay in Qt folder using Qt layout folders:
#    - plugins will go in <qt_folder>/plugins/gammaray
#    - all the other libs, including probe library will go in <qt_folder>/lib
#    - qmake mkspecs file
#  Default=false
#  Only use for Android or when you know what you are doing.
#
# -DGAMMARAY_BUILD_DOCS=[true|false]
#  Build the API documentation and User Manual.
#  Default=true (except when building the probe only)
#
# To build the man page from POD, run 'make man' after CMake (assumes perl is available)
# To install the resulting man page, run 'make install'
# The man page is not available on Windows.
#

project(GammaRay CXX C)

cmake_minimum_required(VERSION 2.8.12) # b/c of ECMGeneratePriFile.cmake
cmake_policy(SET CMP0020 NEW)
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(NOT DEFINED CMAKE_INSTALL_RPATH_USE_LINK_PATH)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
if(NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH TRUE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(GammaRayMacros)
include(GammaRayMacrosInternal)
include(FeatureSummary)
include(ECMGeneratePriFile)
include(ECMEnableSanitizers)
include(GenerateExportHeader)

# Exit for blacklisted compilers (those that don't support C++11 very well)
#  MSVC++ < 2010 aka 10.0 aka 1600
#  Clang 3.0
set(BAD_CXX_MESSAGE "")
if(MSVC)
  if(MSVC_VERSION LESS 1600)
    set(BAD_CXX_MESSAGE "MSVC 2010 or higher")
  endif()
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
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
set(GAMMARAY_VERSION_MINOR "5")
set(GAMMARAY_VERSION_PATCH "50")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")
set(GAMMARAY_SOVERSION "2.6.0")
set(GAMMARAY_PLUGIN_VERSION "2.6")
set(PROJECT_VERSION_STRING "${GAMMARAY_VERSION_STRING}")
if(ANDROID)
  # non-rooted Android doesn't like .so versions...
  set(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES "")
else()
  set(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES SOVERSION ${GAMMARAY_SOVERSION} VERSION ${GAMMARAY_SOVERSION})
endif()

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

set(GAMMARAY_BUILD_UI_DEFAULT ON)
if(QNXNTO OR ANDROID)
  set(GAMMARAY_BUILD_UI_DEFAULT OFF)
endif()
option(GAMMARAY_BUILD_UI "Build the GammaRay client and in-process UI." ${GAMMARAY_BUILD_UI_DEFAULT})

option(
  GAMMARAY_PROBE_ONLY_BUILD
  "Build only an additional probe configuration for an already existing launcher."
  OFF
)

option(GAMMARAY_ENABLE_GPL_ONLY_FEATURES "Enable features only available under GPL license." OFF)

set(GAMMARAY_BUILD_DOCS_DEFAULT ON)
if(${GAMMARAY_PROBE_ONLY_BUILD})
  set(GAMMARAY_BUILD_DOCS_DEFAULT OFF)
endif()
option(GAMMARAY_BUILD_DOCS "Build GammaRay documentation." ${GAMMARAY_BUILD_DOCS_DEFAULT})

#
# Compiler & linker settings
#
add_compiler_export_flags()
if(CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  check_cxx_compiler_flag(-std=gnu++0x HAVE_GXX_GNUXX11)
  check_cxx_compiler_flag(-std=c++0x HAVE_GXX_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type -Wswitch")
  if(HAVE_GXX_GNUXX11) # QNX needs gnu++0x rather than c++0x for compiling QML V4 private headers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  elseif(HAVE_GXX_CXX11)
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
  if(HAVE_GXX_FORMAT_SECURITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security")
  endif()
  if(MINGW)
    # mingw will error out on the crazy casts in probe.cpp without this
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  endif()
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()
if(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
endif()

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE -D_USING_V110_SDK71_=1)
endif()
if(QNXNTO)
  add_definitions(-D_QNX_SOURCE)
endif()

# linker flags
if(NOT ECM_ENABLE_SANITIZERS AND (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU))
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

#
# Finding Qt
#

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
    3DExtras
    3DInput
    3DLogic
    3DRender
    Bluetooth
    Concurrent
    Positioning
    PrintSupport
    Qml
    Quick
    QuickWidgets
    Svg
    Script
    ScriptTools
    Test
    WebKit
    WebKitWidgets
    Widgets
  )
  # if translation/doc host tools are missing, the Qt5 cmake config files throw errors...
  if(NOT CMAKE_CROSSCOMPILING)
    find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS Help LinguistTools)
  endif()

  # starting with 5.6, the Designer module needs CMake 3.0 or higher
  if(Qt5Core_VERSION VERSION_LESS 5.6 OR NOT CMAKE_VERSION VERSION_LESS 3.0)
    find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS Designer)
  endif()
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
  set(HAVE_QT_WEBKIT ${Qt5WebKit_FOUND})
  set(HAVE_QT_WEBKIT1 ${Qt5WebKitWidgets_FOUND})

  if(Qt5_POSITION_INDEPENDENT_CODE AND NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

  # TODO: Remove me once fixed in ECM module
  # more hacks: find qpa/... includes
  # also see https://codereview.qt-project.org/#change,30483
  include_directories(SYSTEM ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

  add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x040800)
  #add_definitions(-DQT_DEPRECATED_WARNINGS)

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
else()
  # Qt4
  set(QT_USE_IMPORTED_TARGETS true)
  find_package(Qt4 4.8.0)
  set_package_properties(Qt4 PROPERTIES TYPE OPTIONAL)
  if(Qt4_FOUND)
    # minimum set
    find_package(Qt4 4.8.0 REQUIRED QtCore QtGui QtNetwork)
  endif()
  find_package(Qt4 4.8.0 QUIET COMPONENTS QtScript QtScriptTools QtWebKit QtDesigner QtSvg QtTest)

  include(${QT_USE_FILE})
  set(HAVE_QT_CONCURRENT true)
  set(HAVE_QT_WIDGETS true)
  set(HAVE_QT_SVG true)
  if(QT_QTDESIGNER_FOUND)
    set(HAVE_QT_DESIGNER true)
  endif()
  set(HAVE_QT_PRINTSUPPORT true)
  set(HAVE_QT_WEBKIT ${QT_QTWEBKIT_FOUND})
  set(HAVE_QT_WEBKIT1 ${QT_QTWEBKIT_FOUND})

  find_path(QT_PRIVATE_INCLUDE_DIR private/qobject_p.h PATHS ${QT_INCLUDES})
  if(QT_PRIVATE_INCLUDE_DIR)
    # not enough, some of them include harfbuzz headers, so we need to find those as well
    # for now we assume a regular Qt4 source build layout, but that probably should be generalized
    find_path(HARFBUZZ_INCLUDE_DIR harfbuzz.h PATH ${QT_PRIVATE_INCLUDE_DIR}/../../src/3rdparty/harfbuzz/src)
  endif()

  if(QT_PRIVATE_INCLUDE_DIR AND HARFBUZZ_INCLUDE_DIR)
    set(HAVE_PRIVATE_QT_HEADERS TRUE)
    include_directories(SYSTEM ${HARFBUZZ_INCLUDE_DIR})
  else()
    set(HAVE_PRIVATE_QT_HEADERS FALSE)
    # needs to go before Qt includes, in case we have non-working headers with the same name there
    include_directories(BEFORE ${CMAKE_SOURCE_DIR}/3rdparty/qt/4.8)
  endif()

  set_package_properties(Qt4 PROPERTIES URL "http://qt-project.org/")
  add_feature_info("QPainter analyzer" HAVE_PRIVATE_QT_HEADERS
    "You must have a build version of Qt available. Make sure the qmake found first in your execute comes from this build version."
  )

  # C++11/Qt5 compatibility
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI\"${CMAKE_SOURCE_DIR}\\compat\\qt4compat.h\"")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include \"${CMAKE_SOURCE_DIR}/compat/qt4compat.h\"")
  endif()

  # Qt4 moc limitation workaround (Qt4 moc cannot evaluate QT_VERSION preprocessor conditionals
  add_definitions(-DQT4_MOC_WORKAROUND)
endif()

# debug suffixes for qmake compatibility
if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "d")
elseif(APPLE)
  set(CMAKE_DEBUG_POSTFIX "_debug")
endif()

if(NOT CMAKE_BUILD_TYPE MATCHES "^[Rr]el")
  set(GAMMARAY_PROBE_ABI_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
endif()

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING)
# disable QT_STRICT_ITERATORS on the Qt5+Windows combo
# see: https://bugreports.qt-project.org/browse/QTBUG-29608
if(NOT (Qt5Core_FOUND AND WIN32))
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

if(HAVE_QT_WIDGETS)
  option(GAMMARAY_CORE_ONLY_LAUNCHER "Only use QtCore in the CLI launcher (breaks style injector, but is needed for Boot2Qt compatibility)" FALSE)
else()
  set(GAMMARAY_CORE_ONLYL_LAUNCHER TRUE)
endif()

add_feature_info("QtScript debugger" QT_QTSCRIPTTOOLS_FOUND "Requires QtScript and QtScriptTools.")
add_feature_info("Web inspector" HAVE_QT_WEBKIT1 "Requires QtWebKit.")
add_feature_info("Widget .ui file export" HAVE_QT_DESIGNER "Requires QtDesigner library.")

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
check_symbol_exists(backtrace execinfo.h HAVE_BACKTRACE)
check_cxx_symbol_exists(abi::__cxa_demangle cxxabi.h HAVE_CXA_DEMANGLE)

# ELF header for ABI detection
find_file(HAVE_ELF_H elf.h)
find_file(HAVE_SYS_ELF_H sys/elf.h)
if(HAVE_ELF_H OR HAVE_SYS_ELF_H)
  set(HAVE_ELF TRUE)
endif()
add_feature_info("ELF ABI detection" HAVE_ELF "Automatic probe ABI detection on ELF-based systems. Requires elf.h.")

find_package(Glslang)
set_package_properties(Glslang PROPERTIES URL "https://github.com/KhronosGroup/glslang" PURPOSE "Validate GL shader code.")

#
# Determine probe ABI
# this needs to be run after we know exactly what we are building, but is needed for that installation settings
include(GammaRayProbeABI)

#
# Installation settings
#
if(ANDROID)
  set(GAMMARAY_INSTALL_QT_LAYOUT ON)
endif()
if(APPLE)
  set(BUNDLE_APP_NAME "GammaRay.app")

  if(GAMMARAY_INSTALL_QT_LAYOUT)
    set(BUNDLE_INSTALL_DIR "bin")
    set(RESOURCES_INSTALL_DIR "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/Resources")
    set(BIN_INSTALL_DIR "bin")
    set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
    set(INCLUDE_INSTALL_DIR "include/gammaray")
    set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
    set(LIBEXEC_INSTALL_DIR "libexec")
    set(CMAKECONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/GammaRay")
    set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray")
    set(MAN_INSTALL_DIR "man/man1")
    set(DOC_INSTALL_DIR "doc/gammaray")
    set(QCH_INSTALL_DIR "doc")
    set(TRANSLATION_INSTALL_DIR "translations")
  else()
    # Make sure default prefix on mac is /Applications, dunnow why but it does not default to it
    # probably because we do not enabled built in bundle support in the main project
    string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "/usr/local" CMP_RESULT)
    if(CMP_RESULT)
      set(CMAKE_INSTALL_PREFIX "/Applications")
    endif()

    set(BUNDLE_INSTALL_DIR ".")
    set(RESOURCES_INSTALL_DIR "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/Resources")
    set(BIN_INSTALL_DIR "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/MacOS")
    set(LIB_INSTALL_DIR "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/Frameworks")
    set(PLUGIN_INSTALL_DIR "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/PlugIns/gammaray")
    set(LIBEXEC_INSTALL_DIR "${BIN_INSTALL_DIR}")
    set(MAN_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/man/man1")
    set(DOC_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/docs")
    set(QCH_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/docs")
    set(TRANSLATION_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/translations")
    set(INCLUDE_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/include/gammaray")
    set(CMAKECONFIG_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/cmake/GammaRay")
    set(ECM_MKSPECS_INSTALL_DIR "${RESOURCES_INSTALL_DIR}/${ECM_MKSPECS_INSTALL_DIR}")
  endif()
else()
  set(BIN_INSTALL_DIR "bin")
  set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
  set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
  set(INCLUDE_INSTALL_DIR "include/gammaray")
  set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/GammaRay)
  set(DATAROOTDIR share CACHE PATH "Define install directory for read-only architecture-independent data")
  set(XDG_APPS_INSTALL_DIR "${DATAROOTDIR}/applications")
  set(APPDATA_INSTALL_DIR "${DATAROOTDIR}/appdata")
  set(ICON_INSTALL_DIR "${DATAROOTDIR}/icons")
  set(MAN_INSTALL_DIR "${DATAROOTDIR}/man/man1")
  set(QCH_INSTALL_DIR "doc" CACHE STRING "Install location of Qt Assistant help files.")
  if(WIN32)
    set(PLUGIN_INSTALL_DIR "plugins/gammaray")
    set(LIBEXEC_INSTALL_DIR "${BIN_INSTALL_DIR}")
    set(DOC_INSTALL_DIR .)
    set(TRANSLATION_INSTALL_DIR "translations")
  else()
    set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray")
    set(LIBEXEC_INSTALL_DIR "${LIB_INSTALL_DIR}/gammaray/libexec")
    set(DOC_INSTALL_DIR "${DATAROOTDIR}/doc/gammaray/")
    set(TRANSLATION_INSTALL_DIR "${DATAROOTDIR}/gammaray/translations")
  endif()
endif()

if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
  set(PROBE_BASENAME "gammaray_probe")
else()
  if(APPLE) # DYLD_INSERT_LIBRARIES only works with real shared libraries, not modules
    set(PROBE_BASENAME "${CMAKE_SHARED_LIBRARY_PREFIX}gammaray_probe")
  else()
    set(PROBE_BASENAME "${CMAKE_SHARED_MODULE_PREFIX}gammaray_probe")
  endif()
endif()

if(GAMMARAY_INSTALL_QT_LAYOUT)
  set(PROBE_INSTALL_DIR ${LIB_INSTALL_DIR})
  # Android doesn't support sub-directories in lib/
  if(ANDROID)
    set(PROBE_PLUGIN_INSTALL_DIR "lib")
  else()
    set(PROBE_PLUGIN_INSTALL_DIR "plugins/gammaray")
  endif()
  set(PLUGIN_INSTALL_DIR ${PROBE_PLUGIN_INSTALL_DIR})
else()
  set(PROBE_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/${GAMMARAY_PLUGIN_VERSION}/${GAMMARAY_PROBE_ABI}${GAMMARAY_PROBE_ABI_POSTFIX}")
  set(PROBE_PLUGIN_INSTALL_DIR ${PROBE_INSTALL_DIR})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

# set RPATH only when installing to a non-default location and not set in cache
if(NOT CMAKE_INSTALL_RPATH)
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
  list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCLibDir)
  list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
  if(${_isSystemPlatformLibDir} EQUAL -1 AND ${_isSystemCLibDir} EQUAL -1 AND ${_isSystemCxxLibDir} EQUAL -1)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
  endif()
endif()

set(
  INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
  BUNDLE DESTINATION ${BUNDLE_INSTALL_DIR}
)

# "inverse" install dirs, to find the base location again
if(APPLE)
  set(BUNDLE_INSTALL_DIR_PRIVATE "${BUNDLE_INSTALL_DIR}/${BUNDLE_APP_NAME}/Contents/MacOS")
  gammaray_inverse_dir(GAMMARAY_INVERSE_BUNDLE_DIR "${BUNDLE_INSTALL_DIR_PRIVATE}")
endif()
gammaray_inverse_dir(GAMMARAY_INVERSE_BIN_DIR "${BIN_INSTALL_DIR}")
gammaray_inverse_dir(GAMMARAY_INVERSE_LIB_DIR "${LIB_INSTALL_DIR}")
gammaray_inverse_dir(GAMMARAY_INVERSE_PROBE_DIR "${PROBE_INSTALL_DIR}")
gammaray_inverse_dir(GAMMARAY_INVERSE_LIBEXEC_DIR "${LIBEXEC_INSTALL_DIR}")

#
# actually build the stuff
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-gammaray.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-gammaray.h
)
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_BINARY_DIR}
)

if(Qt5Core_FOUND OR Qt4_FOUND)
include(QtInstallPaths) #to set QT_INSTALL_FOO variables
add_subdirectory(cmake)
add_subdirectory(3rdparty/kde)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(probe)
add_subdirectory(launcher)
if(GAMMARAY_BUILD_UI AND (Qt5Widgets_FOUND OR QT_QTGUI_FOUND))
  add_subdirectory(ui)
  add_subdirectory(inprocessui)
  if(NOT GAMMARAY_PROBE_ONLY_BUILD)
    add_subdirectory(client)
    add_subdirectory(app)
  endif()
endif()
if((Qt5Test_FOUND OR QT_QTTEST_FOUND) AND NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(tests)
endif()
add_subdirectory(plugins)
add_subdirectory(resources)
if(Qt5LinguistTools_FOUND)
  add_subdirectory(translations)
endif()
if(GAMMARAY_BUILD_DOCS)
  add_subdirectory(docs) # needs to go last, so see all installed headers for the API docs
endif()
endif()

set(LICENSE_FILE "LICENSE.GPL.txt")
set(README_FILE "ReadMe.txt")
list(APPEND DOCS ${LICENSE_FILE} ${README_FILE} "LICENSE.txt" "LICENSE.US.txt" "ReadMe-commercial.txt")
if(NOT APPLE AND NOT GAMMARAY_PROBE_ONLY_BUILD)
  if(UNIX AND GAMMARAY_BUILD_UI)
    install(FILES GammaRay.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES GammaRay.appdata.xml DESTINATION ${APPDATA_INSTALL_DIR})
  endif()
  install(FILES ${DOCS} DESTINATION ${DOC_INSTALL_DIR})
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
    COMMAND ${CPPCHECK_EXECUTABLE} --enable=all --inconclusive -f --suppress=*:${QT_INCLUDE_DIR}* ${_cppcheck_flags}
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
if(NOT GAMMARAY_PROBE_ONLY_BUILD)
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
endif()

#CPACK: General Settings
set(CPACK_GENERATOR "TBZ2")
set(CPACK_PACKAGE_NAME "gammaray")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An introspection tool for Qt applications")
#TODO: shorten lines in the Readme.txt to make rpmlint happy
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/${README_FILE}")
set(CPACK_PACKAGE_VENDOR "Klaralvdalens Datakonsult AB (KDAB)")
set(CPACK_PACKAGE_CONTACT "gammaray-devel@kdab.com")
set(CPACK_PACKAGE_VERSION_MAJOR "${GAMMARAY_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${GAMMARAY_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${GAMMARAY_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${GAMMARAY_VERSION}")

#CPACK: RPM Specific Settings
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2+")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")

#CPACK: DEB Specific Settings
set(CPACK_DEBIAN_PACKAGE_SECTION "Development")

set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${README_FILE}")
if(WIN32)
  set(CPACK_GENERATOR "NSIS" "ZIP")
  set(CPACK_PACKAGE_EXECUTABLES "GammaRay" "GammaRay")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "GammaRay")
  set(CPACK_PACKAGE_FILE_NAME "GammaRay ${GAMMARAY_VERSION}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/${LICENSE_FILE}")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
  set(CPACK_NSIS_MUI_ICON "${ICONS_DIR}/Charm.ico")
  set(CPACK_PACKAGE_ICON "${ICONS_DIR}\\\\CharmNSISHeader.bmp")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.kdab.com/")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "GammaRay${CMAKE_EXECUTABLE_SUFFIX}")
  set(CPACK_NSIS_MENU_LINKS
    "${LICENSE_FILE}" "License"
    "${README_FILE}" "Readme"
  )
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_NSIS_INSTALLED_ICON_NAME}")
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_DMG_VOLUME_NAME "GammaRay")
  set(CPACK_SYSTEM_NAME "OSX")
  set(CPACK_PACKAGE_FILE_NAME "GammaRay-${GAMMARAY_VERSION}")
  set(CPACK_PACKAGE_ICON "${ICONS_DIR}/CharmDMG.icns")
  set(CPACK_DMG_DS_STORE "${ICONS_DIR}/CharmDSStore")
  set(CPACK_DMG_BACKGROUND_IMAGE "${ICONS_DIR}/CharmDMGBackground.png")
elseif(UNIX)
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

include(CPack)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

if(NOT Qt5Core_FOUND AND NOT Qt4_FOUND)
  message(FATAL_ERROR "GammaRay needs either Qt5 or Qt4!")
endif()
