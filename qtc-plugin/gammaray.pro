TEMPLATE = lib
TARGET = GammaRay

# DEFINES += 

PROVIDER = KDAB

include($$QTC_SOURCE_DIR/src/qtcreatorplugin.pri)
include(gammaray_dependencies.pri)

LIBS += -L$$IDE_PLUGIN_PATH/Nokia

# QT += 

# GammaRay files

SOURCES += \
    gammarayplugin.cpp \
    gammaraytool.cpp \
    gammarayengine.cpp \
    gammarayruncontrolfactory.cpp \
    gammaraysettings.cpp \
    gammarayconfigwidget.cpp

HEADERS += \
    gammarayplugin.h \
    gammaraytool.h \
    gammarayengine.h \
    gammarayconstants.h \
    gammarayruncontrolfactory.h \
    gammaraysettings.h \
    gammarayconfigwidget.h

FORMS += \
    gammarayconfigwidget.ui

#RESOURCES += \
