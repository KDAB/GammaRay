# command line launcher
set(gammaray_runner_srcs
  main.cpp
)

gammaray_add_win_icon(gammaray_runner_srcs)

add_executable(gammaray ${gammaray_runner_srcs})

target_link_libraries(gammaray gammaray_launcher gammaray_common)
if(HAVE_QT_WIDGETS)
  target_link_libraries(gammaray Qt::Gui Qt::Widgets)
endif()

gammaray_embed_info_plist(gammaray ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
gammaray_set_rpath(gammaray ${BIN_INSTALL_DIR})

install(TARGETS gammaray ${INSTALL_TARGETS_DEFAULT_ARGS})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:gammaray>/$<TARGET_PDB_FILE_NAME:gammaray>" DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
