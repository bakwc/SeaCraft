#-------------------------------------------------
#
# Project created by QtCreator 2012-06-21T19:04:16
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Field.cpp \
    Images.cpp \
    Model.cpp \
    Controller.cpp \
    Utility.cpp

HEADERS  += mainwindow.h \
    Field.h \
    Images.h \
    Model.h \
    Controller.h \
    Utility.h \
    Defines.h

FORMS    += mainwindow.ui
