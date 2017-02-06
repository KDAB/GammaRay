macro(qt5_build_doc _qdocconf_name)
  #compute the qdoc template dir from where the qt-html-templates-offline.qdocconf was found
  get_filename_component(QDOC_TEMPLATE_DIR ${QDOC_TEMPLATE} DIRECTORY)

  #compute the qdoc index dir from where the qtcore.index was found
  get_filename_component(QDOC_INDEX_DIR ${QDOC_QTCORE_INDEX} DIRECTORY)
  get_filename_component(QDOC_INDEX_DIR ${QDOC_INDEX_DIR} DIRECTORY)

  # pre-process .qdocconf
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}.qdocconf.in ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}.qdocconf)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-offline.qdocconf.in ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-offline.qdocconf)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online.qdocconf.in ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online.qdocconf)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_qdocconf_name}-online-kdab.qdocconf.in ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online-kdab.qdocconf)

  # offline docs
  # run qdoc to get qhp file
  set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name})
  # FIXME: this should rather be taken from the qdocconf file?
  file(GLOB_RECURSE _qdoc_srcs ${CMAKE_CURRENT_SOURCE_DIR} "*.qdoc")
  file(GLOB _qdoc_imgs ${CMAKE_CURRENT_SOURCE_DIR} "images/*.png")
  add_custom_command(
    OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qhp
    COMMAND ${QDOC_EXECUTABLE}
      --indexdir ${QDOC_INDEX_DIR}
      --outputdir ${_qdoc_output_dir}
    ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-offline.qdocconf
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-offline.qdocconf ${_qdoc_srcs} ${_qdoc_imgs}
  )

  # generate qch file from qhp
  add_custom_command(
    OUTPUT ${_qdoc_output_dir}/${_qdocconf_name}.qch
    COMMAND ${QHELPGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}/${_qdocconf_name}.qhp
    DEPENDS ${_qdoc_output_dir}/${_qdocconf_name}.qhp
  )
  add_custom_target(${_qdocconf_name}.qch ALL DEPENDS ${_qdoc_output_dir}/${_qdocconf_name}.qch)

  # install qch
  install(FILES ${_qdoc_output_dir}/${_qdocconf_name}.qch DESTINATION ${QCH_INSTALL_DIR})

  # online docs
  file(GLOB _qdoc_styles ${CMAKE_CURRENT_SOURCE_DIR} "style/*")
  # Qt style
  set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online)
  add_custom_command(
    OUTPUT ${_qdoc_output_dir}/index.html
    COMMAND ${QDOC_EXECUTABLE}
      --indexdir ${QDOC_INDEX_DIR}
      --outputdir ${_qdoc_output_dir}
    ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online.qdocconf
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online.qdocconf ${_qdoc_srcs} ${_qdoc_imgs} ${_qdoc_styles}
  )
  add_custom_target(online-docs DEPENDS ${_qdoc_output_dir}/index.html)
  # KDAB style
  set(_qdoc_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online-kdab)
  add_custom_command(
    OUTPUT ${_qdoc_output_dir}/index.html
    COMMAND ${QDOC_EXECUTABLE}
      --indexdir ${QDOC_INDEX_DIR}
      --outputdir ${_qdoc_output_dir}
    ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online-kdab.qdocconf
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_qdocconf_name}-online-kdab.qdocconf ${_qdoc_srcs} ${_qdoc_imgs} ${_qdoc_styles}
  )
  add_custom_target(online-docs-kdab DEPENDS ${_qdoc_output_dir}/index.html)

endmacro()

find_file(QTQUICK_INDEX qtquick/qtquick.index
  HINTS ${QT_INSTALL_DOCS}
  HINTS ${QT_INSTALL_DATA}
  HINTS ${QT_INSTALL_DATA}/doc
)
if(QTQUICK_INDEX)
  set(HAVE_QTQUICK_DOCS "qtquick")
else()
  message(STATUS "qdoc qtquick.index not found: User Manual will be missing links to QtQuick API references")
endif()

find_file(QTQML_INDEX qtqml/qtqml.index
  HINTS ${QT_INSTALL_DOCS}
  HINTS ${QT_INSTALL_DATA}
  HINTS ${QT_INSTALL_DATA}/doc
)
if(QTQML_INDEX)
  set(HAVE_QTQML_DOCS "qtqml")
else()
  message(STATUS "qdoc qml.index not found: User Manual will be missing links to QML API references")
endif()

find_file(QT3D_INDEX qt3d/qt3d.index
  HINTS ${QT_INSTALL_DOCS}
  HINTS ${QT_INSTALL_DATA}
  HINTS ${QT_INSTALL_DATA}/doc
)
if(QT3D_INDEX)
  set(HAVE_QT3D_DOCS "qt3d")
else()
  message(STATUS "qdoc qt3d.index not found: User Manual will be missing links to Qt3D API references")
endif()

qt5_build_doc(gammaray-manual)
