QT += core
QT -= gui

CONFIG += c++11

TARGET = minipkgen
CONFIG += console
CONFIG -= app_bundle
DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY \
           QT_MESSAGELOGCONTEXT

TEMPLATE = app

SOURCES += main.cpp \
    diffgenerator.cpp \
    ../common/logger.cpp

HEADERS += \
    diffgenerator.h \
    ../common/logger.h \
    options.h \
    ../common/fileentry.h \
    ../common/defines.h
