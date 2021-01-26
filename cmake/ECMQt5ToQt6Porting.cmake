#.rst:
# ECMQt5ToQt6Porting
# ------------------
#
# When included in a Qt5 context this module creates version-less targets
# for Qt5 library and executable targets. Qt 5.15 and Qt 6 provides such
# targets by default already, this module helps with supporting more Qt
# versions with significantly reduced conditional code in the build system.
#
# Both public and private library targets are handled, as well as a number
# of related variables (_FOUND, _VERSION_*).
#
# Since 5.79.0.
#=============================================================================
# SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

macro(_create_versionless_library_target LIB)
    if (NOT TARGET Qt::${LIB} AND TARGET Qt5::${LIB})
        add_library(Qt::${LIB} INTERFACE IMPORTED)
        set_target_properties(Qt::${LIB} PROPERTIES
            INTERFACE_LINK_LIBRARIES Qt5::${LIB}
            _qt_is_versionless_target "TRUE"
        )
    endif()
    if (NOT TARGET Qt::${LIB}Private AND TARGET Qt5::${LIB}Private)
        add_library(Qt::${LIB}Private INTERFACE IMPORTED)
        set_target_properties(Qt::${LIB}Private PROPERTIES
            INTERFACE_LINK_LIBRARIES Qt5::${LIB}Private
        )
    endif()

    set(_vars
        VERSION
        VERSION_MAJOR
        VERSION_MINOR
        VERSION_PATCH
        FOUND
        DEFINITIONS
    )
    foreach (var ${_vars})
        if (NOT DEFINED Qt${LIB}_${var} AND DEFINED Qt5${LIB}_${var})
            set(Qt${LIB}_${var} ${Qt5${LIB}_${var}})
        endif()
        if (NOT DEFINED Qt${LIB}_${var} AND DEFINED Qt6${LIB}_${var})
            set(Qt${LIB}_${var} ${Qt6${LIB}_${var}})
        endif()
    endforeach()
    unset(_vars)
endmacro()

macro(_create_versionless_executable_target EXE)
    if(NOT TARGET Qt::${EXE} AND TARGET Qt5::${EXE})
        add_executable(Qt::${EXE} IMPORTED)
        # see Qt's cmake config files for this list of properties
        foreach(_prop
                IMPORTED_LOCATION
                IMPORTED_LOCATION_RELEASE
                IMPORTED_LOCATION_RELWITHDEBINFO
                IMPORTED_LOCATION_MINSIZEREL
                IMPORTED_LOCATION_DEBUG)
            get_target_property(_location Qt5::${EXE} ${_prop})
            if(_location AND EXISTS "${_location}")
                break()
            endif()
        endforeach()
        set_target_properties(Qt::${EXE} PROPERTIES IMPORTED_LOCATION "${_location}")
        unset(_location)
    endif()
endmacro()

set(_libs
    3DAnimation
    3DCore
    3DExtras
    3DInput
    3DLogic
    3DQuick
    3DQuickAnimation
    3DQuickExtras
    3DQuickInput
    3DQuickRender
    3DQuickScene2D
    3DRender
    AndroidExtras
    Bluetooth
    Charts
    Concurrent
    Core
    DBus
    Designer
    DesignerComponents
    EglFSDeviceIntegration
    EglFsKmsSupport
    Gui
    Gui_GLESv2
    Help
    HunspellInputMethod
    IviCore
    IviMedia
    IviVehicleFunctions
    LinguistTools
    Location
    Mqtt
    Multimedia
    MultimediaGstTools
    MultimediaQuick
    MultimediaWidgets
    Network
    NetworkAuth
    Nfc
    OpenGL
    Pdf
    PdfWidgets
    Positioning
    PositioningQuick
    PrintSupport
    Qml
    QmlModels
    QmlWorkerScript
    Quick
    QuickControls2
    QuickParticles
    QuickShapes
    QuickTemplates2
    QuickTest
    QuickWidgets
    RemoteObjects
    Scxml
    Sensors
    SerialBus
    SerialPort
    Sql
    Svg
    Test
    TextToSpeech
    VirtualKeyboard
    UiPlugin
    UiTools
    WaylandClient
    WaylandCompositor
    WebChannel
    WebEngine
    WebEngineCore
    WebEngineWidgets
    WebSockets
    Widgets
    X11Extras
    Xml
)

foreach(lib ${_libs})
    _create_versionless_library_target(${lib})
endforeach()
unset(_libs)

set(_exes
    lconvert
    lprodump
    lrelease
    lupdate
    moc
    qdbuscpp2xml
    qdbusxml2cpp
    qdoc
    qhelpgenerator
    qmake
    qmlcachegen
    qmllint
    qmlplugindump
    qmltestrunner
    qmltyperegistrar
    qlalr
    qtattributionsscanner
    qvkgen
    qtwaylandscanner
    rcc
    uic
)

foreach(exe ${_exes})
    _create_versionless_executable_target(${exe})
endforeach()
unset(_exes)
