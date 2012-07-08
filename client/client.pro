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

CONFIG += static

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    Field.cpp \
    Images.cpp \
    Model.cpp \
    Controller.cpp \
    Utility.cpp \
    ConnectionInfoDialog.cpp \
    AboutDialog.cpp

HEADERS += \
    mainwindow.h \
    Field.h \
    Images.h \
    Model.h \
    Controller.h \
    Utility.h \
    Defines.h \
    ConnectionInfoDialog.h \
    AboutDialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    russian.ts \
    ukrainian.ts

langsRelease.commands = lrelease $${_PRO_FILE_}
QMAKE_EXTRA_TARGETS += langsRelease
PRE_TARGETDEPS += langsRelease

# PlaySound utility
HEADERS += util/PlaySound.h
SOURCES += util/PlaySound.cpp

CONFIG( unix ) {
    LIBS += -lasound
    SOURCES += util/PlaySound_nix.cpp
    HEADERS += util/PlaySound_nix.h
}

CONFIG( windows ) {
    SOURCES += util/PlaySound_win.cpp
    HEADERS += util/PlaySound_win.h
}
