QT += core
QT -= gui

CONFIG += c++11

TARGET = increments
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    cliquenetworkmanager.cpp \
    tournament.cpp \
    utils.cpp \
    converter.cpp \
    cliquemodule.cpp \
    cliquemoduleintelligence.cpp \
    classifier.cpp \
    testmodule.cpp \
    cliquenetwork.cpp \
    comparer.cpp

HEADERS += \
    cliquenetworkmanager.h \
    tournament.h \
    utils.h \
    converter.h \
    cliquemodule.h \
    cliquemoduleintelligence.h \
    classifier.h \
    testmodule.h \
    cltransformations.h \
    cliquenetwork.h \
    comparer.h
