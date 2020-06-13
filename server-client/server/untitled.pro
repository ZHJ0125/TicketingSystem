#-------------------------------------------------
#
# Project created by QtCreator 2019-07-08T00:42:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    globalapi.h \
    servicethread.h \
    threadbuff.h \
    ticket.h

FORMS    += mainwindow.ui
