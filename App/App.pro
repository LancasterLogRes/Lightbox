CONFIG -= qt
TARGET = App
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lCommon -lEGL -lGLESv2
x86: LIBS += -lX11
android: LIBS += $$LIBSTL -llog -landroid

SOURCES += AppEngine.cpp \
    Display.cpp \
    App.cpp
HEADERS +=    AppEngine.h \
    Display.h \
    App.h \
    All.h
