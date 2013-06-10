TEMPLATE = lib
include ( ../Common.pri )

HEADERS += StreamEvent.h \
    EventCompilerLibrary.h \
    EventCompilerImpl.h \
    EventCompiler.h \
    Track.h \
    EventType.h \
	Character.h \
	Compiler.h \
    GraphSpec.h

SOURCES += \
    Track.cpp \
	StreamEvent.cpp \
	Compiler.cpp \
    GraphSpec.cpp

SOURCES += \
    EventCompilerImpl.cpp
