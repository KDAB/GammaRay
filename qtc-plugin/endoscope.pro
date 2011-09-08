TEMPLATE = lib
TARGET = Endoscope

# DEFINES += 

include($$QTC_SOURCE_DIR/src/qtcreatorplugin.pri)
include(endoscope_dependencies.pri)

# QT += 

# Endoscope files

SOURCES += \
    endoscopeplugin.cpp \
    endoscopetool.cpp \
    endoscopeengine.cpp \
    endoscoperuncontrolfactory.cpp \
    endoscopesettings.cpp \
    endoscopeconfigwidget.cpp

HEADERS += \
    endoscopeplugin.h \
    endoscopetool.h \
    endoscopeengine.h \
    endoscopeconstants.h \
    endoscoperuncontrolfactory.h \
    endoscopesettings.h \
    endoscopeconfigwidget.h

FORMS += \
    endoscopeconfigwidget.ui

#RESOURCES += \
