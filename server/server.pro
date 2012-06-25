#-------------------------------------------------
#
# Project created by QtCreator 2012-06-21T10:07:16
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    MainServer.cpp \
    Field.cpp \
    Statistics.cpp

HEADERS += \
    MainServer.h \
    Field.h \
    Statistics.h
