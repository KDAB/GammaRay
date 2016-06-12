set(LANGUAGES de)

# ts file update (manual target, affects source dir)
add_custom_target(ts)
foreach(LANG ${LANGUAGES})
    add_custom_target(
        ts_${LANG}
        COMMAND Qt5::lupdate
            3rdparty app client common core inprocessui launcher plugins ui
            -locations relative -no-ui-lines -no-obsolete -ts ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_dependencies(ts ts_${LANG})
endforeach()

# qm generation and installation
foreach(LANG ${LANGUAGES})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
        COMMAND Qt5::lrelease ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
            -qm ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
    )
    add_custom_target(gammaray_${LANG}.qm ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm DESTINATION ${TRANSLATION_INSTALL_DIR})
endforeach()
