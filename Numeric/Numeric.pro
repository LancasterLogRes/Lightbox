CONFIG -= qt
TARGET = LGL
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lCommon

SOURCES += Colour.cpp
HEADERS += Colour.h Coord.h Matrix.h Vector.h Rect.h Quaternion.h \
    LGL.h All.h ../Numeric.h
