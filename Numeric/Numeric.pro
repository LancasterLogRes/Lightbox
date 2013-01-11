TEMPLATE = lib
include ( ../Common.pri )

PRE_TARGETDEPS += ../Common
LIBS += -lCommon
SOURCES += \
    Vector.cpp
HEADERS += Coord.h Matrix.h Vector.h Rect.h Quaternion.h All.h ../Numeric.h \
    Ellipse.h
