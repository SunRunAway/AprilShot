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
    rectselector.cpp \
    modebox.cpp \
    titlebar.cpp \
    qiniu.cpp

HEADERS  += \
    mainwidget.h \
    utils.h \
    rectselector.h \
    modebox.h \
    titlebar.h \
    qiniu.h \
    rs.h \
    utils.h \
    titlebar.h \
    modebox.h \
    rectselector.h


FORMS    += \
    mainwidget.ui \
    titlebar.ui

RC_FILE = icon.rc

OTHER_FILES += \
    icon.rc \
    AprilShot.ico \

RESOURCES += \
    icon.qrc \
    systray.qrc
