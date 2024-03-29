# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

# Use various Qt tools to generate the manuals, in both Qt and KDAB branding
macro(qt_build_doc _qdocconf_name)

    # run the attribution scanner to collect 3rdparty license information
    file(GLOB_RECURSE _qt_attributions "${CMAKE_SOURCE_DIR}/3rdparty/*/qt_attribution.json")
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codecontributions.qdoc
        COMMAND ${QTATTRIBUTIONSSCANNER_EXECUTABLE} --output ${CMAKE_CURRENT_BINARY_DIR}/codecontributions.qdoc --filter
                QDocModule=gammaray-manual ${CMAKE_SOURCE_DIR}
        DEPENDS ${_qt_attributions} ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-offline.qdocconf
        COMMENT "Generate codecontrbutions.qdoc using qtattributescanner"
    )

    # Point to the qt documentation index files, if discovered
    set(_qdoc_index_args)
    if(QDOC_INDEX_DIR)
        set(_qdoc_index_args --indexdir ${QDOC_INDEX_DIR})
    endif()

    # offline docs
    # run qdoc to get qhp file
    set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name})
    # FIXME: this should rather be taken from the qdocconf file?
    file(GLOB_RECURSE _qdoc_srcs ${CMAKE_CURRENT_SOURCE_DIR} "*.qdoc")
    file(GLOB _qdoc_imgs ${CMAKE_CURRENT_SOURCE_DIR} "images/*.png")
    list(APPEND _qdoc_srcs "${CMAKE_CURRENT_BINARY_DIR}/codecontributions.qdoc")
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qhp
        COMMAND
            ${CMAKE_COMMAND} -E env QT_INSTALL_DOCS=${QDOC_TEMPLATE_DIR} QT_VERSION_TAG=${GAMMARAY_PLUGIN_VERSION}
            QT_VERSION=${GAMMARAY_VERSION} BUILDDIR=${CMAKE_CURRENT_BINARY_DIR} ${QDOC_EXECUTABLE} ${_qdoc_index_args}
            --outputdir ${_qdoc_output_dir} ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-offline.qdocconf
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-offline.qdocconf ${_qdoc_srcs} ${_qdoc_imgs}
        COMMENT "Generate the offline manual using qdoc"
    )

    # generate qch file from qhp
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qch
        COMMAND ${QHELPGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}/${_qdocconf_name}.qhp
        DEPENDS ${_qdoc_output_dir}/${_qdocconf_name}.qhp
        COMMENT "Generate the qch version of the manual using qhelpgenerator"
    )
    add_custom_target(
        ${_qdocconf_name}.qch ALL
        DEPENDS ${_qdoc_output_dir}/${_qdocconf_name}.qch
        COMMENT "Target to generate the .qch file"
    )

    # install qch
    install(FILES ${_qdoc_output_dir}/${_qdocconf_name}.qch DESTINATION ${QCH_INSTALL_DIR})

    # online docs
    file(GLOB _qdoc_styles ${CMAKE_CURRENT_SOURCE_DIR} "style/*")
    # Qt style
    set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online)
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/index.html
        COMMAND
            ${CMAKE_COMMAND} -E env QT_INSTALL_DOCS=${QDOC_TEMPLATE_DIR} QT_VERSION_TAG=${GAMMARAY_PLUGIN_VERSION}
            QT_VERSION=${GAMMARAY_VERSION} BUILDDIR=${CMAKE_CURRENT_BINARY_DIR} ${QDOC_EXECUTABLE} ${_qdoc_index_args}
            --outputdir ${_qdoc_output_dir} ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online.qdocconf
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online.qdocconf ${_qdoc_srcs} ${_qdoc_imgs}
                ${_qdoc_styles}
        COMMENT "Generate the Qt-branded online manual using qdoc"
    )
    add_custom_target(
        online-docs
        DEPENDS ${_qdoc_output_dir}/index.html
        COMMENT "Target to generate the Qt-branded online docs"
    )
    # KDAB style
    set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online-kdab)
    add_custom_command(
        OUTPUT ${_qdoc_output_dir}/index.html
        COMMAND
            ${CMAKE_COMMAND} -E env QT_INSTALL_DOCS=${QDOC_TEMPLATE_DIR} QT_VERSION_TAG=${GAMMARAY_PLUGIN_VERSION}
            QT_VERSION=${GAMMARAY_VERSION} BUILDDIR=${CMAKE_CURRENT_BINARY_DIR} ${QDOC_EXECUTABLE} ${_qdoc_index_args}
            --outputdir ${_qdoc_output_dir} ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online-kdab.qdocconf
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online-kdab.qdocconf ${_qdoc_srcs} ${_qdoc_imgs}
                ${_qdoc_styles}
        COMMENT "Generate the KDAB-branded online manual using qdoc"
    )
    add_custom_target(
        online-docs-kdab
        DEPENDS ${_qdoc_output_dir}/index.html
        COMMENT "Target to generate the KDAB-branding online docs"
    )

endmacro()

qt_build_doc(gammaray-manual)
