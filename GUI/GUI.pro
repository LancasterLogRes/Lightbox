CONFIG -= qt
TARGET = GUI
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lApp -lLGL -lEGL -lGLESv2 -lNumeric -lCommon -lpng -lz

RES += Shaders.resources
SOURCES += \
    View.cpp \
    BasicButton.cpp \
    GUIApp.cpp

HEADERS += \
    View.h \
    BasicButton.h \
    Global.h \
    GUIApp.h \
    All.h
OTHER_FILES += Shaders.resources View.frag View.vert
