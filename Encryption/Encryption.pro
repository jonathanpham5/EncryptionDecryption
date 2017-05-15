#-------------------------------------------------
#
# Project created by QtCreator 2016-11-15T14:05:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Encryption
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    encryptionGenerator.cpp

HEADERS  += mainwindow.h \
    encryptionGenerator.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\OpenSSL-Win64\include

LIBS+= -LC:/OpenSSL-Win64/lib -llibcrypto -llibssl

CONFIG += static
