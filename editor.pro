#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T21:00:37
#
#-------------------------------------------------

QT       += core gui
QT       += core
QT       += widgets
QT       += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = editor
TEMPLATE = app


SOURCES += main.cpp \
    image.cpp \
    utility.cpp

HEADERS  += \
    image.h \
    utility.h

CONFIG += c++11
