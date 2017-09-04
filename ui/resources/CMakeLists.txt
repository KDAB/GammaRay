# See https://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

if(UNIX AND NOT APPLE AND GAMMARAY_BUILD_UI)
    foreach(size 16 24 32 48 64 128 256 512)
        foreach(scale 1 2 3)
            set(scaled_size ${size}x${size})

            if(${scale} GREATER 1)
                set(scaled_size ${size}x${size}@${scale})
            endif()

            set(source gammaray/GammaRay-${scaled_size}x.png)

            if(EXISTS ${source})
                install(
                    FILES ${source}
                    DESTINATION "${ICON_INSTALL_DIR}/hicolor/${scaled_size}/apps" RENAME GammaRay.png
                    )
            endif()
        endforeach(scale)
    endforeach(size)
endif()

if(APPLE)
    if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
        install(FILES gammaray/GammaRay.icns DESTINATION "${RESOURCES_INSTALL_DIR}")
    endif()
endif()
