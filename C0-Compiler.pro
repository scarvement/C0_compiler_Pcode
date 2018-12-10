#-------------------------------------------------
#
# Project created by QtCreator 2018-11-18T22:42:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = C0-Compiler
TEMPLATE = app


SOURCES += main.cpp\
    src/mainwindow.cpp \
    src/Token.cpp \
    src/opg.cpp \
    src/grammar.cpp

HEADERS  += include/mainwindow.h \
    include/Token.h \
    include/opg.h \
    include/grammar.h

FORMS    += mainwindow.ui
