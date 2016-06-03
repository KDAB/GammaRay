# TODO: some of this probably should be upstreamed to Qt's cmake files...

find_program(QDOC_EXECUTABLE qdoc)

find_file(QDOC_TEMPLATE global/qt-html-templates-offline.qdocconf
  HINTS ${QT_INSTALL_DOCS}
  HINTS ${QT_INSTALL_DATA}
  HINTS ${QT_INSTALL_DATA}/doc
)

find_file(QDOC_QTCORE_INDEX qtcore.index
  HINTS ${QT_INSTALL_DOCS}/qtcore
  HINTS ${QT_INSTALL_DATA}/doc/qtcore
)

if(NOT QDOC_EXECUTABLE OR NOT QHELPGEN_EXECUTABLE OR NOT QDOC_TEMPLATE OR NOT QDOC_QTCORE_INDEX)
  message(STATUS "Unable to build user manual in qch format.")
  message(STATUS "qdoc exectuable: ${QDOC_EXECUTABLE}")
  message(STATUS "qhelpgenerator exectuable: ${QHELPGEN_EXECUTABLE}")
  message(STATUS "qdoc template: ${QDOC_TEMPLATE}")
  message(STATUS "qdoc qtcore index: ${QDOC_QTCORE_INDEX}")
  set(HAVE_USER_MANUAL_BUILD False)
else()
  set(HAVE_USER_MANUAL_BUILD True)
endif()

add_feature_info("User Manual creation in qch format" HAVE_USER_MANUAL_BUILD "Requires qdoc, qhelpgenerator, the qdoc templates and index files")

macro(qt5_build_doc _qdocconf_in_file)
  #compute the qdoc template dir from where the qt-html-templates-offline.qdocconf was found
  get_filename_component(QDOC_TEMPLATE_DIR ${QDOC_TEMPLATE} DIRECTORY)

  #compute the qdoc index dir from where the qtcore.index was found
  get_filename_component(QDOC_INDEX_DIR ${QDOC_QTCORE_INDEX} DIRECTORY)
  get_filename_component(QDOC_INDEX_DIR ${QDOC_INDEX_DIR} DIRECTORY)

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

if(HAVE_USER_MANUAL_BUILD)
  qt5_build_doc(gammaray-manual.qdocconf.in)
endif()
