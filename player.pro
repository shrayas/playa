#-------------------------------------------------
#
# Project created by QtCreator 2013-11-28T18:17:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    manager.cpp \
    settingswindow.cpp \
    lastfm_helper.cpp

HEADERS  += mainwindow.h \
    manager.h \
    player.h \
    settingswindow.h \
    lastfm_helper.h

FORMS    += mainwindow.ui \
    settingswindow.ui

LIBS += -lvlc -ltag -lsqlite3 -lcurl -lssl -ljsonxx -lcrypto
CONFIG += c++11
