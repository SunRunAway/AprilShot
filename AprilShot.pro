#-------------------------------------------------
#
# Project created by QtCreator 2012-12-09T04:16:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AprilShot
TEMPLATE = app


SOURCES += main.cpp\
    mainwidget.cpp \
    utils.cpp \
    rectselector.cpp

HEADERS  += \
    mainwidget.h \
    utils.h \
    rectselector.h

FORMS    += \
    mainwidget.ui

RC_FILE = icon.rc