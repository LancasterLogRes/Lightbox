#-------------------------------------------------
#
# Project created by QtCreator 2011-12-08T10:09:13
#
#-------------------------------------------------

TEMPLATE = lib
include ( ../Common.pri )

LIBS += -lCommon
PRE_TARGETDEPS += ../Common

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
