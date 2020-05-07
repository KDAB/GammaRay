include(ExternalProject)

# on Windows we need to build the debug/release counter-part as well
if(NOT WIN32)
  return()
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]el")
  set(_build_type "Debug")
else()
  set(_build_type "Release")
endif()

ExternalProject_Add(
  GammaRay-${_build_type}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/${_build_type}
  EXCLUDE_FROM_ALL 1
  STEP_TARGETS build install

  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${_build_type}
    -DGAMMARAY_MULTI_BUILD=OFF
    -DGAMMARAY_PROBE_ONLY_BUILD=ON
    -DGAMMARAY_BUILD_CLI_INJECTOR=OFF
    -DGAMMARAY_INSTALL_QT_LAYOUT=${GAMMARAY_INSTALL_QT_LAYOUT}
    -DGAMMARAY_BUILD_UI=${GAMMARAY_BUILD_UI}
    -DGAMMARAY_OUTPUT_PREFIX=${CMAKE_BINARY_DIR}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DBUILD_TESTING=OFF
    -DCMAKE_OBJECT_PATH_MAX=512
)

# hacky way to add a dependency from the default target to the build step
add_custom_target(multibuild_build ALL DEPENDS GammaRay-${_build_type}-build)

# even hackier way to postpone external project installation to install time
add_custom_target(multibuild_install)
add_dependencies(multibuild_install GammaRay-${_build_type}-install)
_ep_get_step_stampfile(GammaRay-${_build_type} "install" install_stamp_file)
if(install_stamp_file)
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E remove \"${install_stamp_file}\")")
endif()
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target multibuild_install)")
