#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T09:57:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BootableDriveMaker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    guihandler.cpp

HEADERS  += mainwindow.h \
    guihandler.h

FORMS    += mainwindow.ui

ICON = icons/BootableDriveMaker.icns
