# probe plugin
if (NOT GAMMARAY_CLIENT_ONLY_BUILD)
set(gammaray_network_srcs
    networksupport.cpp
    networkinterfacemodel.cpp
    networkreplymodel.cpp

    cookies/cookieextension.cpp
    cookies/cookiejarmodel.cpp
)
if (TARGET Qt5::Network)
    list(APPEND gammaray_network_srcs networkconfigurationmodel.cpp)
endif()
gammaray_add_plugin(gammaray_network JSON gammaray_network.json SOURCES ${gammaray_network_srcs})
target_link_libraries(gammaray_network gammaray_core Qt::Network)
endif()

# ui plugin
if(GAMMARAY_BUILD_UI)
  set(gammaray_network_ui_srcs
    networkwidget.cpp
    networkinterfacewidget.cpp
    networkconfigurationwidget.cpp
    clientnetworkconfigurationmodel.cpp
    networkreplywidget.cpp
    clientnetworkreplymodel.cpp

    cookies/cookietab.cpp
  )
  gammaray_add_plugin(gammaray_network_ui JSON gammaray_network.json SOURCES ${gammaray_network_ui_srcs})
  target_link_libraries(gammaray_network_ui gammaray_ui Qt::Network)
endif()
