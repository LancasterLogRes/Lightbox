TEMPLATE = lib
CONFIG += force_shared
include ( ../Common.pri )

LIBS += -lCommon -lEventCompiler
PRE_TARGETDEPS += ../Common ../EventCompiler
SOURCES += ExampleEventCompiler.cpp
HEADERS += ExampleEventCompiler.h
