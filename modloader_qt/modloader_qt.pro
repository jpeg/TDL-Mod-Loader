#-------------------------------------------------
#
# Project created by QtCreator 2012-11-13T00:54:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modloader_qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    configmodifier.cpp

HEADERS  += mainwindow.h \
    configmodifier.h \
    errorcodes.h

FORMS    += mainwindow.ui
