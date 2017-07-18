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
#  Default=true (except on QNX and Android)
#
# -DGAMMARAY_PROBE_ONLY_BUILD=[true|false]
#  Build only an additional probe configuration for an already existing launcher.
#  Default=false
#
# -DGAMMARAY_ENABLE_GPL_ONLY_FEATURES=[true|false]
#  Enable features only available under GPL license.
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
# -DGAMMARAY_MULTI_BUILD=[true|false]
#  Build multiple applicable probe configurations.
#  Default=true
#
# -DGAMMARAY_STATIC_PROBE=[true|false]
#  Build the probe as static library for compile-time injection (requires Qt 5.4 or newer).
#  Default=false
#
# -DGAMMARAY_BUILD_DOCS=[true|false]
#  Build the API documentation and User Manual.
#  Default=true (except when building the probe only)

# -DGAMMARAY_ENFORCE_QT_ASSERTS=[true|false]
#  Force QT_ASSERT in all builds.
#  Default=false
#
# To build the man page from POD, run 'make man' after CMake (assumes perl is available)
# To install the resulting man page, run 'make install'
# The man page is not available on Windows.
#

project(GammaRay CXX C)

if(MSVC)
  cmake_minimum_required(VERSION 3.1.0) # b/c of TARGET_PDB_FILE_DIR generator expression
else()
  cmake_minimum_required(VERSION 3.0) # b/c of AUTOUIC/AUTORCC
endif()

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW)
endif()
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
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

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CMakeParseArguments)
include(CMakePushCheckState)
include(GammaRayMacros)
include(GammaRayMacrosInternal)
include(FeatureSummary)
include(ECMGeneratePriFile)
include(ECMEnableSanitizers)
include(GenerateExportHeader)
include("3rdparty/backward-cpp/BackwardMacros.cmake")

# Exit for blacklisted compilers (those that don't support C++11 very well)
#  MSVC++ < 2010 aka 10.0 aka 1600
#  Clang 3.0
set(BAD_CXX_MESSAGE "")
if(MSVC)
  if(MSVC_VERSION LESS 1600)
    set(BAD_CXX_MESSAGE "MSVC 2010 or higher")
  endif()
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.1.0)
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
set(GAMMARAY_VERSION_MINOR "8")
set(GAMMARAY_VERSION_PATCH "50")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")
set(GAMMARAY_SOVERSION "2.9.0")
set(GAMMARAY_PLUGIN_VERSION "2.9")
set(PROJECT_VERSION_STRING "${GAMMARAY_VERSION_STRING}")
if(ANDROID)
  # non-rooted Android doesn't like .so versions and requires the lib prefix
  set(GAMMARAY_DEFAULT_LIBRARY_PROPERTIES PREFIX "lib")
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
gammaray_option(
  GAMMARAY_ENFORCE_QT4_BUILD
  "Enforce a build with Qt4 (by default GammaRay looks out for Qt5 first)"
  OFF
)

set(GAMMARAY_BUILD_UI_DEFAULT ON)
if(QNXNTO OR ANDROID)
  set(GAMMARAY_BUILD_UI_DEFAULT OFF)
endif()
gammaray_option(GAMMARAY_BUILD_UI "Build the GammaRay client and in-process UI." ${GAMMARAY_BUILD_UI_DEFAULT})

gammaray_option(
  GAMMARAY_PROBE_ONLY_BUILD
  "Build only an additional probe configuration for an already existing launcher."
  OFF
)

gammaray_option(GAMMARAY_ENABLE_GPL_ONLY_FEATURES "Enable features only available under GPL license." OFF)
gammaray_option(GAMMARAY_INSTALL_QT_LAYOUT "Install into Qt directory layout." OFF)
gammaray_option(GAMMARAY_MULTI_BUILD "Build multiple applicable probe configurations." ON)

set(GAMMARAY_BUILD_DOCS_DEFAULT ON)
if(${GAMMARAY_PROBE_ONLY_BUILD})
  set(GAMMARAY_BUILD_DOCS_DEFAULT OFF)
endif()
gammaray_option(GAMMARAY_BUILD_DOCS "Build GammaRay documentation." ${GAMMARAY_BUILD_DOCS_DEFAULT})
gammaray_option(
  GAMMARAY_STATIC_PROBE
  "Build the probe as static library for compile-time injection (requires Qt 5.4 or newer)."
  OFF
)

gammaray_option(
  GAMMARAY_ENFORCE_QT_ASSERTS
  "Force QT_ASSERT in all builds."
  OFF
)

gammaray_option(GAMMARAY_DISABLE_FEEDBACK "Disable user feedback support." OFF)

#
# Static probe setup
#
if(GAMMARAY_STATIC_PROBE)
  set(GAMMARAY_BUILD_UI OFF)
  set(GAMMARAY_PROBE_ONLY_BUILD ON)
  set(GAMMARAY_ENFORCE_QT4_BUILD OFF)

  set(GAMMARAY_LIBRARY_TYPE STATIC)
  set(GAMMARAY_PLUGIN_TYPE STATIC)
else()
  set(GAMMARAY_LIBRARY_TYPE SHARED)
  set(GAMMARAY_PLUGIN_TYPE MODULE)
endif()

#
# Compiler & linker settings
#
function(append_if condition value)
  if(${condition})
    foreach(variable ${ARGN})
      set(${variable} "${${variable}} ${value}" PARENT_SCOPE)
    endforeach()
  endif()
endfunction()

macro(add_flag_if_supported flag name)
  check_c_compiler_flag("-Werror ${flag}" "C_SUPPORTS_${name}")
  append_if("C_SUPPORTS_${name}" "${flag}" CMAKE_C_FLAGS)
  check_cxx_compiler_flag("-Werror ${flag}" "CXX_SUPPORTS_${name}")
  append_if("CXX_SUPPORTS_${name}" "${flag}" CMAKE_CXX_FLAGS)
endmacro()

function(check_private_headers_exist module private_includes_var)
    message(STATUS "Checking whether private include directories for module ${module} exist")
    foreach(_dir ${private_includes_var})
        if (NOT EXISTS "${_dir}")
            message(FATAL_ERROR "The private include directory ${_dir} for module ${module} do not exist! Please make sure your Qt5 installation contains private headers.\nThe required directories:\n  ${private_includes_var}")
        endif()
    endforeach()
endfunction()
function(check_private_header_exists _file _dirs)
    set(_found FALSE)
    foreach(_dir ${_dirs})
        if(EXISTS "${_dir}/${_file}")
            set(_found TRUE)
        endif()
    endforeach()
    if(NOT _found)
        message(FATAL_ERROR "The private include file ${_file} was not found in directory ${_dirs}! Please make sure your Qt5 installation contains private headers.")
    endif()
endfunction()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_flag_if_supported(-Wunused-but-set-variable UNUSED_BUT_SET)
  add_flag_if_supported(-Wlogical-op LOGICAL_OP)
  add_flag_if_supported(-Wsizeof-pointer-memaccess POINTER_MEMACCESS)
  add_flag_if_supported(-Wreorder REORDER)
  add_flag_if_supported(-Wformat-security FORMAT_SECURITY)

  check_cxx_compiler_flag(-std=gnu++0x HAVE_GXX_GNUXX11)
  check_cxx_compiler_flag(-std=c++0x HAVE_GXX_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type -Wswitch")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # -Wgnu-zero-variadic-macro-arguments (part of -pedantic) is triggered by every qCDebug() call and therefore results
    # in a lot of noise. This warning is only notifying us that clang is emulating the GCC behaviour
    # instead of the exact standard wording so we can safely ignore it
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
  endif()
  if(HAVE_GXX_GNUXX11) # QNX needs gnu++0x rather than c++0x for compiling QML V4 private headers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  elseif(HAVE_GXX_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
  if(MINGW)
    # mingw will error out on the crazy casts in probe.cpp without this
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  endif()
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -Wdocumentation")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments -Wdocumentation")
endif()

# Do not treat the operator name keywords and, bitand, bitor, compl, not, or and xor as synonyms as keywords.
# They're not supported under Visual Studio out of the box thus using them limits the portability of code
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER_ID MATCHES "Clang" OR (CMAKE_C_COMPILER_ID STREQUAL "Intel" AND NOT WIN32))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-operator-names")
endif()

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE -D_USING_V110_SDK71_=1)
endif()
if(QNXNTO)
  add_definitions(-D_QNX_SOURCE)
endif()

# linker flags
if(NOT ECM_ENABLE_SANITIZERS AND (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU))
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

if (GAMMARAY_ENFORCE_QT_ASSERTS)
  add_definitions(-DQT_FORCE_ASSERTS)
endif()

#
# Finding Qt
#

# try Qt5 first, and prefer that (if found), but only if not disabled via option
if(NOT GAMMARAY_ENFORCE_QT4_BUILD)
  if(GAMMARAY_STATIC_PROBE)
    # static probes are only available with Qt 5.4 or newer
    find_package(Qt5Core 5.4 NO_MODULE REQUIRED)
  else()
    find_package(Qt5Core 5.1 QUIET)
  endif()
else()
  set(Qt5Core_FOUND FALSE)
endif()

if(Qt5Core_FOUND)
  set_package_properties(Qt5Core PROPERTIES TYPE REQUIRED)
  find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Gui Network Xml)
  find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS
    3DExtras
    3DInput
    3DLogic
    3DRender
    3DQuick
    Bluetooth
    Concurrent
    Designer
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
    WaylandCompositor
  )
  # Find these 'exotic' Qt modules without using find_package(... COMPONENTS)
  # so we can retrieve those packages even when installed into a different prefix
  find_package(Qt5IviCore 1.1 NO_MODULE QUIET) # 1.1 is based on 5.8
  find_package(Qt5IviVehicleFunctions 1.1 NO_MODULE QUIET)
  find_package(Qt5IviMedia 1.1 NO_MODULE QUIET)
  find_package(Qt5Scxml 5.8 NO_MODULE QUIET)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND Qt5Core_VERSION VERSION_LESS 5.5)
    # disable warnings about inconsistent usage of 'override' specifier for Qt <5.5
    # only starting with 5.5 Qt started to use 'override' throughout the code base
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
  endif()
  if(Qt5Core_VERSION VERSION_GREATER 5.7)
    # Qt 5.8 is the first minor version to use nullptr more broadly
    # With versions below Qt might lots of warnings for -Wzero-as-null-pointer-constant, e.g.:
    #   .../examples/widget-layouting/ui_contactform.h:606:85: warning: zero as null pointer constant [-Wzero-as-null-pointer-constant]
    add_flag_if_supported(-Wzero-as-null-pointer-constant ZERO_AS_NULL_POINTER_CONSTANT)
  endif()

  # if translation/doc host tools are missing, the Qt5 cmake config files throw errors...
  if(NOT CMAKE_CROSSCOMPILING)
    find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS Help LinguistTools)
  endif()

  if(GAMMARAY_BUILD_UI) # widgets are required for the UI
    find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Widgets)
    # 3D Widget inspector requires Qt3D 5.7
    if(Qt5Core_VERSION VERSION_EQUAL 5.7 OR Qt5Core_VERSION VERSION_GREATER 5.7)
      if(${Qt53DRender_FOUND} AND ${Qt53DInput_FOUND} AND ${Qt53DLogic_FOUND} AND ${Qt53DQuick_FOUND})
        set(GAMMARAY_WITH_WIDGET3D TRUE)
      else()
        message(STATUS "Not building GammaRay Widget 3D")
      endif()
    endif()
  endif()

  # Sanity checking, we need private includes for the following modules
  check_private_headers_exist("Qt5Core" "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
  check_private_header_exists("private/qobject_p.h" "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
  check_private_headers_exist("Qt5Gui" "${Qt5Gui_PRIVATE_INCLUDE_DIRS}")
  if(Qt5Qml_FOUND)
    check_private_headers_exist("Qt5Qml" "${Qt5Qml_PRIVATE_INCLUDE_DIRS}")
  endif()
  if(Qt5Quick_FOUND)
    check_private_headers_exist("Qt5Quick" "${Qt5Quick_PRIVATE_INCLUDE_DIRS}")
    check_private_header_exists("private/qquickitem_p.h" "${Qt5Quick_PRIVATE_INCLUDE_DIRS}")
  endif()
  set(HAVE_PRIVATE_QT_HEADERS TRUE)

  include("cmake/ECMQt4To5Porting.cmake")

  set(HAVE_QT_CONCURRENT ${Qt5Concurrent_FOUND})
  set(HAVE_QT_WIDGETS ${Qt5Widgets_FOUND})
  set(HAVE_QT_SVG ${Qt5Svg_FOUND})
  set(HAVE_QT_SCXML ${Qt5Scxml_FOUND})
  set(HAVE_QT_DESIGNER ${Qt5Designer_FOUND})

  if(Qt5PrintSupport_FOUND)
    cmake_push_check_state()
    set(CMAKE_REQUIRED_LIBRARIES Qt5::PrintSupport)
    check_cxx_symbol_exists(QT_NO_PRINTER "QPrinter" QT_NO_PRINTER)
    cmake_pop_check_state()
    if(NOT QT_NO_PRINTER)
      set(HAVE_QT_PRINTSUPPORT ON)
    endif()
  endif()
  set(HAVE_QT_WEBKIT ${Qt5WebKit_FOUND})
  set(HAVE_QT_WEBKIT1 ${Qt5WebKitWidgets_FOUND})

  add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x040800)
  #add_definitions(-DQT_DEPRECATED_WARNINGS)

  set_package_properties(Qt5 PROPERTIES URL "https://qt.io/")
  set_package_properties(Qt5Concurrent PROPERTIES TYPE RECOMMENDED PURPOSE "Required for the GammaRay launcher process list.")
  set_package_properties(Qt5Widget PROPERTIES TYPE RECOMMENDED PURPOSE "Required for the GammaRay client UI and widget-related tools.")
  set_package_properties(Qt5Svg PROPERTIES TYPE OPTIONAL PURPOSE "Required for widget SVG export.")
  set_package_properties(Qt5PrintSupport PROPERTIES TYPE OPTIONAL PURPOSE "Required for widget PDF export.")
else()
  # Qt4
  set(QT_USE_IMPORTED_TARGETS true)
  find_package(Qt4 4.8.0)
  set_package_properties(Qt4 PROPERTIES TYPE OPTIONAL)
  if(Qt4_FOUND)
    # minimum set
    find_package(Qt4 4.8.0 REQUIRED QtCore QtGui QtNetwork QtXml)
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

  set_package_properties(Qt4 PROPERTIES URL "https://qt.io/")
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

add_definitions(
  -DQT_USE_FAST_CONCATENATION
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_URL_CAST_FROM_STRING
)

# disable QT_STRICT_ITERATORS on the >=Qt5.0<=Qt5.2+Windows combo
# see: https://bugreports.qt-project.org/browse/QTBUG-29608
if(NOT (Qt5Core_FOUND AND Qt5Core_VERSION VERSION_LESS 5.3 AND WIN32))
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

if(HAVE_QT_WIDGETS)
  gammaray_option(GAMMARAY_CORE_ONLY_LAUNCHER "Only use QtCore in the CLI launcher (breaks style injector, but is needed for Boot2Qt compatibility)" FALSE)
else()
  set(GAMMARAY_CORE_ONLY_LAUNCHER TRUE)
endif()

add_feature_info("QtScript debugger" QT_QTSCRIPTTOOLS_FOUND "Requires QtScript and QtScriptTools.")
add_feature_info("Web inspector" HAVE_QT_WEBKIT1 "Requires QtWebKit.")
add_feature_info("Widget .ui file export" HAVE_QT_DESIGNER "Requires QtDesigner library.")
add_feature_info("3D Widget Inspector" GAMMARAY_WITH_WIDGET3D "Requires Qt5 >= 5.7, Qt3D and QtQuick Controls")

#
# Additional dependencies
#

if(NOT WIN32 AND NOT QNXNTO AND NOT ANDROID)
  set(DL_LIBRARY dl)
endif()

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
check_include_files(unistd.h HAVE_UNISTD_H)
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

find_package(QmlLint)
set_package_properties(QmlLint PROPERTIES URL "https://qt.io" PURPOSE "Validate QML code.")

if(Qt5Core_FOUND)
  find_package(KF5SyntaxHighlighting 5.28 NO_MODULE QUIET)
  set_package_properties(KF5SyntaxHighlighting PROPERTIES TYPE RECOMMENDED URL "https://www.kde.org/" PURPOSE "Syntax highlighting for code editor.")
  if(TARGET KF5::SyntaxHighlighting)
    set(HAVE_SYNTAX_HIGHLIGHTING TRUE)
  endif()
endif()

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
  set(PROBE_BASENAME "${CMAKE_SHARED_LIBRARY_PREFIX}gammaray_probe")
endif()

if(GAMMARAY_INSTALL_QT_LAYOUT)
  if(WIN32)
    set(PROBE_INSTALL_DIR ${BIN_INSTALL_DIR})
  else()
    set(PROBE_INSTALL_DIR ${LIB_INSTALL_DIR})
  endif()
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

if(NOT GAMMARAY_OUTPUT_PREFIX) # set by multibuild
  set(GAMMARAY_OUTPUT_PREFIX ${PROJECT_BINARY_DIR})
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GAMMARAY_OUTPUT_PREFIX}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GAMMARAY_OUTPUT_PREFIX}/${LIB_INSTALL_DIR})

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
  if(NOT GAMMARAY_DISABLE_FEEDBACK)
    add_subdirectory(3rdparty/kuserfeedback)
  endif()
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
if(Qt5LinguistTools_FOUND)
  add_subdirectory(translations)
endif()
if(GAMMARAY_MULTI_BUILD)
  add_subdirectory(multibuild)
endif()
if(GAMMARAY_BUILD_DOCS)
  add_subdirectory(examples)
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
  set(ICONS_DIR "${CMAKE_SOURCE_DIR}/resources")
  set(CPACK_GENERATOR "NSIS" "ZIP")
  set(CPACK_PACKAGE_EXECUTABLES "GammaRay" "GammaRay")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "GammaRay")
  set(CPACK_PACKAGE_FILE_NAME "GammaRay ${GAMMARAY_VERSION}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/${LICENSE_FILE}")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
  set(CPACK_NSIS_MUI_ICON "${ICONS_DIR}/GammaRay.ico")
  #set(CPACK_PACKAGE_ICON "${ICONS_DIR}\\\\CharmNSISHeader.bmp")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://www.kdab.com/")
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
