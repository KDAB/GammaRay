# See https://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
if(UNIX AND NOT APPLE AND GAMMARAY_BUILD_UI)
  foreach(size 16 24 32 48 64 128 256 512)
    foreach(scale 1 2 3)
      set(scaled_size ${size}x${size})
      set(source gammaray/GammaRay-${scaled_size}.png)

      if(${scale} GREATER 1)
        set(scaled_size ${size}x${size}@${scale})
        set(source gammaray/GammaRay-${scaled_size}x.png)
      endif()

      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source})
        install(
          FILES ${CMAKE_CURRENT_SOURCE_DIR}/${source}
          DESTINATION "${ICON_INSTALL_DIR}/hicolor/${scaled_size}/apps" RENAME GammaRay.png
        )
      endif()
    endforeach()
  endforeach()
endif()

if(APPLE)
  if(NOT GAMMARAY_INSTALL_QT_LAYOUT)
    install(FILES gammaray/GammaRay.icns DESTINATION "${RESOURCES_INSTALL_DIR}")
  endif()
endif()

# generate the class icon qrc file
file(READ "classiconindex.map" _map_content)
string(REPLACE "\n" ";" _map_content ${_map_content})
set(QRC_CONTENT "")
foreach(_icon_data IN LISTS _map_content)
  if(_icon_data)
    string(REPLACE " " ";" _icon_data ${_icon_data})
    list(GET _icon_data 0 _icon_name)
    list(GET _icon_data 1 _icon_file)
    set(QRC_CONTENT "${QRC_CONTENT}\n        <file alias=\"${_icon_name}\">${CMAKE_CURRENT_SOURCE_DIR}/gammaray/ui/icons/classes/${_icon_file}</file>")
  endif()
endforeach()
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/classicons.qrc.in
  ${CMAKE_CURRENT_BINARY_DIR}/classicons.qrc
)
