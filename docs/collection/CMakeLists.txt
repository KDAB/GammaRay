if(NOT TARGET Qt5::qcollectiongenerator)
    message(STATUS "qcollectiongenerator not found, documention collection will not be generated.")
    return()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gammaray.qhcp.cmake ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhcp)

# TODO conditionally enable API docs inclusion here
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/../manual/gammaray-manual/gammaray-manual.qch ${CMAKE_CURRENT_BINARY_DIR}
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/../api/qch/gammaray-api.qch ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND Qt5::qcollectiongenerator ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhcp -o ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/gammaray.qhcp.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/../manual/gammaray-manual/gammaray-manual.qch
#        ${CMAKE_CURRENT_BINARY_DIR}/../api/qch/gammaray-api.qch
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(gammaray.qhc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray.qhc DESTINATION ${QCH_INSTALL_DIR})
