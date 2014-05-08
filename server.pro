#-------------------------------------------------
#
# Project created by QtCreator 2014-04-23T18:05:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dispatcher.cpp \
    server.cpp \
    graphics.cpp

HEADERS  += mainwindow.h \
    dispatcher.h \
    server.h \
    graphics.h

FORMS    += mainwindow.ui
LIBS     += -lws2_32
