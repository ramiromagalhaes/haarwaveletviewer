#-------------------------------------------------
#
# Project created by QtCreator 2013-07-17T15:42:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = haarwaveletviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -L/home/ramiro/workspace/build-ecrsgen-Debug/ -lhaarwavelet -lopencv_core -lopencv_imgproc -lopencv_highgui
#-lboost_serialization -lboost_filesystem -lboost_system
