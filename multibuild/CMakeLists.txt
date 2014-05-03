include(ExternalProject)

# on MSVC we need to build the debug/release counter-part as well
if(MSVC)
  if(CMAKE_BUILD_TYPE MATCHES "^[Rr]el")
    set(_build_type "Debug")
  else()
    set(_build_type "Release")
  endif()

  ExternalProject_Add(
    GammaRay-${_build_type}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/GammaRay-${_build_type}
    INSTALL_DIR ${CMAKE_BINARY_DIR}

    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${_build_type}
      -DGAMMARAY_PROBE_ONLY_BUILD=ON
      -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
      -DLIB_SUFFIX=${LIB_SUFFIX}
  )

endif()
