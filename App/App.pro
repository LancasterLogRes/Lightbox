TEMPLATE = lib
include ( ../Common.pri )

LIBS += $$GFX_LIBS
android: LIBS += $$LIBSTL -llog -landroid

SOURCES += Display.cpp
HEADERS += Display.h \
	All.h \
	Global.h

!pi: SOURCES += AppEngine.cpp \
    App.cpp
!pi: HEADERS +=    AppEngine.h \
	App.h
