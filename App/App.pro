CONFIG -= qt
TARGET = App
TEMPLATE = lib
include ( ../Common.pri )
LIBS +=  -lCommon
android:LIBS += -lCommon $$LIBSTL -lLGL -lEGL -lGLESv2 -llog -landroid -lpng -lz

SOURCES += AppEngine.cpp \
    Display.cpp \
    App.cpp
HEADERS +=    AppEngine.h \
    Display.h \
    App.h \
    All.h
