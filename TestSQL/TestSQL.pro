#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T13:58:29
#
#-------------------------------------------------

QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestSQL
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    smsdialog.cpp \
    audio.cpp \
    database.cpp

HEADERS  += mainwindow.h \
    smsdialog.h \
    audio.h \
    database.h

FORMS    += mainwindow.ui \
    smsdialog.ui

RESOURCES += \
    buttons.qrc
