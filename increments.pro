QT += core
QT -= gui

CONFIG += c++11

TARGET = increments
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    cliquenetwork.cpp \
    cliquenetworkmanager.cpp \
    tournament.cpp \
    utils.cpp \
    converter.cpp \
    cliquemodule.cpp

HEADERS += \
    cliquenetwork.h \
    cliquenetworkmanager.h \
    tournament.h \
    utils.h \
    converter.h \
    cliquemodule.h
