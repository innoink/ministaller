QT += core
QT -= gui

CONFIG += c++11

TARGET = minipkgen
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    diffgenerator.cpp

HEADERS += \
    diffgenerator.h
