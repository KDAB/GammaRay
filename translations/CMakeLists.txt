set(LANGUAGES en de)

# translation tools
add_executable(ljsonupdate ljsonupdate.cpp)
target_link_libraries(ljsonupdate Qt::Core)

# ts file update (manual target, affects source dir)
file(GLOB_RECURSE jsons "${CMAKE_SOURCE_DIR}/plugins/*.json")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp
    COMMAND ljsonupdate --extract --out ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp ${jsons}
    DEPENDS ${jsons}
)

add_custom_target(ts)
foreach(LANG ${LANGUAGES})
    add_custom_target(
        ts_${LANG}
        COMMAND Qt::lupdate
            3rdparty app client common core inprocessui launcher plugins ui ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp
            -locations relative -no-ui-lines -no-obsolete -ts ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plugin_meta_data.cpp
    )
    add_dependencies(ts ts_${LANG})
endforeach()

# qm generation and installation
foreach(LANG ${LANGUAGES})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
        COMMAND Qt::lrelease ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
            -qm ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gammaray_${LANG}.ts
    )
    add_custom_target(gammaray_${LANG}_qm ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm DESTINATION ${TRANSLATION_INSTALL_DIR})
endforeach()

# message merging (manual target, affects source dir)
add_custom_target(tsmerge)
foreach(LANG ${LANGUAGES})
    add_custom_target(
        tsmerge_${LANG}
        COMMAND ljsonupdate --merge
            --qm ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
            --lang ${LANG} ${jsons}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gammaray_${LANG}.qm
    )
    add_dependencies(tsmerge tsmerge_${LANG})
endforeach()
