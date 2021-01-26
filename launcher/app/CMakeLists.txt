set(gammaray_launcher_ui_srcs
    main.cpp
    ${CMAKE_SOURCE_DIR}/resources/gammaray.qrc
)

gammaray_add_win_icon(gammaray_launcher_ui_srcs)

add_executable(gammaray-launcher WIN32 ${gammaray_launcher_ui_srcs})
target_link_libraries(gammaray-launcher
  Qt::Gui Qt::Widgets
  gammaray_common
  gammaray_launcher_ui
)

gammaray_embed_info_plist(gammaray-launcher ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
set_target_properties(gammaray-launcher PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${LIBEXEC_INSTALL_DIR}"
)
gammaray_set_rpath(gammaray-launcher ${LIBEXEC_INSTALL_DIR})

install(TARGETS gammaray-launcher DESTINATION ${LIBEXEC_INSTALL_DIR})
