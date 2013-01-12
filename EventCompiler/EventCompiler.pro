TEMPLATE = lib
include ( ../Common.pri )

HEADERS += StreamEvent.h \
    EventCompilerLibrary.h \
    EventCompilerImpl.h \
    EventCompiler.h \
    Preprocessors.h \
    Track.h \
    EventType.h \
    Character.h

SOURCES += \
    Preprocessors.cpp \
    Track.cpp \
    StreamEvent.cpp
