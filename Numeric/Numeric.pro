CONFIG -= qt
TARGET = Numeric
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lCommon

SOURCES += \
    Vector.cpp
HEADERS += Coord.h Matrix.h Vector.h Rect.h Quaternion.h All.h ../Numeric.h
