TARGET = qmldbg_tcp
QT       += declarative network

include(../../qpluginbase.pri)

QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/qmltooling
QTDIR_build:REQUIRES += "contains(QT_CONFIG, declarative)"

SOURCES += \
    qtcpserverconnection.cpp

HEADERS += \
    qtcpserverconnection.h

target.path += $$[QT_INSTALL_PLUGINS]/qmltooling
INSTALLS += target

symbian:TARGET.UID3=0x20031E90
