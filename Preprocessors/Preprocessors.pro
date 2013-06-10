TEMPLATE = lib
include ( ../Common.pri )

HEADERS += Common.h Preprocessors.h \
    Spectral.h \
    Bark.h \
    Surface.h \
    Downsampled.h \
    Historical.h \
    Arithmetical.h \
    CrossComparison.h \
    Normalization.h \
    Peaks.h \
    Flow.h \
    Tonal.h \
    Analysis.h

SOURCES += Preprocessors.cpp
