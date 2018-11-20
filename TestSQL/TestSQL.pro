#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T13:58:29
#llibtagd было
#-------------------------------------------------

QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestSQL
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    smsdialog.cpp \
    database.cpp \
    mynetworktelegram.cpp \
    audioplayeer.cpp

HEADERS  += mainwindow.h \
    smsdialog.h \
    database.h \
    mynetworktelegram.h \
    audioplayeer.h

FORMS    += mainwindow.ui \
    smsdialog.ui

RESOURCES += \
    buttons.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -llibtag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -llibtag

INCLUDEPATH += $$PWD/taglib
DEPENDPATH += $$PWD/taglib

include($$PWD/telegram/QtTelegramBot.pri)
