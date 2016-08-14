TEMPLATE = app

QMAKE_MAC_SDK = macosx10.11
QT += qml quick
CONFIG += c++11
DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += main.cpp \
    packageinstaller.cpp \
    packageparser.cpp \
    ../common/logger.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    packageinstaller.h \
    platform.h \
    packageparser.h \
    ../common/logger.h \
    ../common/fileentry.h \
    ../common/defines.h
