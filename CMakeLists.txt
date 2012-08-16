# This is the top-level CMakeLists.txt file for the GammaRay project.
#
# Pass the following variables to cmake to control the build:
#
# -DGAMMARAY_UNKNOWN_CXX_MANGLED_NAMES=[on|off]
#  Set this if your compiler uses an unsupported C++ name mangling scheme
#  Default=off
#
# To build the man page from POD, run 'make man' after CMake (assumes perl is available)
# To install the resulting man page, run 'make install'
# Not available on Windows.
#
# To build the apidox, run 'make docs' after CMake (assumes doxygen is available)
# Not available on Windows.
#

project(GammaRay)
cmake_minimum_required(VERSION 2.8)

if(NOT Prog_NAME)
  set(Prog_NAME "GammaRay")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})

set(GAMMARAY_VERSION_MAJOR "1")
set(GAMMARAY_VERSION_MINOR "2")
set(GAMMARAY_VERSION_PATCH "1")
set(GAMMARAY_VERSION "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}")
set(GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION}")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

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
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DQT_STRICT_ITERATORS)

include(CheckCXXCompilerFlag)
include(CheckLibraryExists)
include(GammaRayMacros)
include(MacroLogFeature)

set(QT_MIN_VERSION "4.7.0")
find_package(Qt5Transitional REQUIRED
  Core
  Gui
  Svg
  Test
)

# TODO: Remove me once fixed in ECM module
if(Qt5Core_FOUND)
  # Avoid errors and build in PIC mode:
  # qt5/qtbase/include/QtCore/qglobal.h:1765:4: error:
  #   #error "You must build your code with position independent code if Qt was
  #   built with -reduce-relocations. " "Compile your code with -fPIC or -fPIE."
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  # more hacks: find qpa/... includes
  # also see https://codereview.qt-project.org/#change,30483
  include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
set(PLUGIN_INSTALL_DIR "${LIB_INSTALL_DIR}/qt4/plugins")

set(
  INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
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
  QT_QTWEBKIT_FOUND
  "WebKit in Qt"
  "A Qt-based web browser engine"
  "included with Qt"
  FALSE
  ""
  "Required for the webinspector tool"
)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wmissing-noreturn -Werror=return-type -fvisibility=hidden")
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

add_subdirectory(core)
add_subdirectory(launcher)

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
if(GRAPHVIZ_FOUND)
  add_definitions(-DGRAPHVIZ_MAJOR_VERSION=${GRAPHVIZ_MAJOR_VERSION} -DGRAPHVIZ_MINOR_VERSION=${GRAPHVIZ_MINOR_VERSION})
endif()

#VTK discovery works a lot better if you give CMake a hint using the VTK_DIR variable
find_path(VTK_DIR VTKConfig.cmake
  /usr/lib64/vtk /usr/lib/vtk /usr/local/lib64/vtk /usr/local/lib/vtk
)
find_package(VTK)
macro_log_feature(
  VTK_FOUND
  "VTK"
  "Visualization Toolkit"
  "http://www.vtk.org"
  FALSE
  ""
  "Needed for the object visualization plugin"
)

if(UNIX AND NOT APPLE)
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
  #man page generation using pod2man
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/gammaray.1
    COMMAND pod2man -c "KDAB Products" -r "\"${GAMMARAY_VERSION}\"" -s 1 ${CMAKE_SOURCE_DIR}/gammaray.pod ${CMAKE_BINARY_DIR}/gammaray.1
    DEPENDS ${CMAKE_SOURCE_DIR}/gammaray.pod
  )
  add_custom_target(man ALL DEPENDS ${CMAKE_BINARY_DIR}/gammaray.1)

  install(FILES ${CMAKE_BINARY_DIR}/gammaray.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)

  file(GLOB API_LIST include/*.h include/*.dox images/*.html)

  #apidox generation using doxygen
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/apidocs/html/index.html
    COMMAND echo \"PROJECT_NUMBER = ${GAMMARAY_VERSION}\" > ${CMAKE_BINARY_DIR}/versfoo
    COMMAND echo \"INPUT = ${CMAKE_SOURCE_DIR}/include\" > ${CMAKE_BINARY_DIR}/inputfoo
    COMMAND echo \"IMAGE_PATH = ${CMAKE_SOURCE_DIR}/images\" > ${CMAKE_BINARY_DIR}/imgfoo
    COMMAND echo \"HTML_FOOTER = ${CMAKE_SOURCE_DIR}/images/footer.html\" > ${CMAKE_BINARY_DIR}/footfoo
    COMMAND cat ${CMAKE_SOURCE_DIR}/Doxyfile
                ${CMAKE_BINARY_DIR}/versfoo
                ${CMAKE_BINARY_DIR}/inputfoo
                ${CMAKE_BINARY_DIR}/imgfoo
                ${CMAKE_BINARY_DIR}/footfoo | doxygen -
    DEPENDS ${API_LIST} ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(docs
    DEPENDS ${CMAKE_BINARY_DIR}/apidocs/html/index.html
  )

endif()

include(ExternalProject)

add_subdirectory(include)
add_subdirectory(tests)
add_subdirectory(plugins)

macro_display_feature_log()
