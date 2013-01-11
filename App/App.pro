TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lCommon $$GFX_LIBS
android: LIBS += $$LIBSTL -llog -landroid

SOURCES += AppEngine.cpp \
    Display.cpp \
    App.cpp
HEADERS +=    AppEngine.h \
    Display.h \
    App.h \
    All.h \
    Global.h
