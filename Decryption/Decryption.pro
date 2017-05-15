#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T09:46:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Decryption
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
