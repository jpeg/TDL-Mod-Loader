#-------------------------------------------------
#
# Project created by QtCreator 2012-11-13T00:54:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modloader_qt
TEMPLATE = app

RC_FILE = application.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    configmodifier.cpp \
    modmanager.cpp \
    about.cpp \
    settings.cpp \
    updater.cpp

HEADERS  += mainwindow.h \
    configmodifier.h \
    errorcodes.h \
    modmanager.h \
    about.h \
    settings.h \
    updater.h

FORMS    += mainwindow.ui \
    about.ui \
    settings.ui

RESOURCES += \
    icons.qrc

LIBS += quazip1.dll

OTHER_FILES += \
    application.rc
