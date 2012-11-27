CONFIG -= qt
TARGET = GUI
TEMPLATE = lib
include ( ../Common.pri )

SOURCES += \
    View.cpp \
    BasicButton.cpp
HEADERS += \
    View.h \
    BasicButton.h \
    Global.h
