# TODO: some of this probably should be upstreamed to Qt's cmake files...

find_program(QDOC_EXECUTABLE qdoc)
find_program(QHELPGEN_EXECUTABLE qhelpgenerator)
find_file(QDOC_TEMPLATE doc/global/qt-html-templates-offline.qdocconf)

if (NOT QDOC_EXECUTABLE OR NOT QHELPGEN_EXECUTABLE OR NOT QDOC_TEMPLATE)
    message(STATUS "Unable to build documentation.")
    return()
endif()

macro(qt5_build_doc _qdocconf_in_file)
    get_filename_component(QDOC_TEMPLATE_DIR ${QDOC_TEMPLATE} DIRECTORY)
    get_filename_component(QDOC_INDEX_DIR ${QDOC_TEMPLATE_DIR} DIRECTORY)

    # pre-process .qdocconf
    get_filename_component(_qdocconf_name ${_qdocconf_in_file} NAME_WE)
    set(_qdocconf_file ${_qdocconf_name}.qdocconf)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_in_file} ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_file})

    # run qdoc to get qhp file
    set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name})
    # FIXME: this should rather be taken from the qdocconf file?
    file(GLOB _qdoc_srcs ${CMAKE_CURRENT_SOURCE_DIR} "*.qdoc")
    file(GLOB _qdoc_imgs ${CMAKE_CURRENT_SOURCE_DIR}/images "*.png")
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qhp
        COMMAND ${QDOC_EXECUTABLE}
            --indexdir ${QDOC_INDEX_DIR}
            --outputdir ${_qdoc_output_dir}
            ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_file}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_file} ${_qdoc_srcs} ${_qdoc_imgs}
    )

    # generate qch file from qhp
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qch
        COMMAND ${QHELPGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}/${_qdocconf_name}.qhp
        DEPENDS ${_qdoc_output_dir}/${_qdocconf_name}.qhp
    )
    add_custom_target(${_qdocconf_name} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}/${_qdocconf_name}.qch)

    # install qch
    install(FILES ${_qdoc_output_dir}/${_qdocconf_name}.qch DESTINATION ${QCH_INSTALL_DIR})
endmacro()

qt5_build_doc(gammaray-manual.qdocconf.in)
