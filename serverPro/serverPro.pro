#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T15:09:05
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET    = serverPro
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE  = app

SOURCES += main.cpp \
    FieldPro.cpp \
    Server.cpp \
    Application.cpp

HEADERS += \
    FieldPro.h \
    Server.h \
    Application.h \
    constants.h
