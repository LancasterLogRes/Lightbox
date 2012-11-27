CONFIG -= qt
TARGET = GUI
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lEGL -lGLESv2 -lCommon -lNumeric -lpng -lz

RES += Shaders.resources
SOURCES += \
    View.cpp \
    BasicButton.cpp
HEADERS += \
    View.h \
    BasicButton.h \
    Global.h \
    All.h
OTHER_FILES += Shaders.resources View.frag View.vert
