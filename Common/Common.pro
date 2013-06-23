TEMPLATE = lib
CONFIG += static
include ( ../Common.pri )

# required for non-windows platforms, it seems...
!win32: !android: LIBS += -lboost_system

# for windows, it doesn't seem to find libboost_system, so we do it manually.
win32 {
	debug: LIBS += -lboost_system-mgw46-mt-d-1_50
	release: LIBS += -lboost_system-mgw46-mt-1_50
}

!android: LIBS += $$FFTW3_LIBS
android: LIBS += -llog

SOURCES += Common.cpp \
	FFT.cpp \
    Maths.cpp \
    StreamIO.cpp \
    Global.cpp \
    Color.cpp \
    Peaks.cpp \
    Time.cpp \
    RGBA8.cpp \
    RGBA.cpp \
	thread.cpp \
	fix_fft.cpp
#	fixmath.cpp

HEADERS += Common.h Global.h Angular.h \
    Time.h \
    GraphParameters.h \
    WavHeader.h \
    Algorithms.h \
    Color.h \
    Trivial.h \
    BoolArray.h \
	Fixed.h \
	FFT.h \
    UnitTesting.h \
    Maths.h \
    StreamIO.h \
    Statistics.h \
    Peaks.h \
    Pimpl.h \
    Flags.h \
    DoTypes.h \
    MemberCollection.h \
    MemberMap.h \
    Members.h \
    GL.h \
    RGBA8.h \
    RGBA8.h \
    RGBA.h \
	thread.h \
	fix_fft.h \
    SimpleKey.h
#	fixmath.h
