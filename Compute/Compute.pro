TEMPLATE = lib
include ( ../Common.pri )

HEADERS += Common.h Compute.h \
    Accumulate.h \
    All.h \
    ZeroCrossings.h WindowedFourier.h ExtractMagnitude.h \
    ExtractPhase.h \
    Bark.h \
    SumRecord.h \
    MeanRecord.h \
    PeakGather.h \
    PeakTrack.h \
    CycleDelta.h

SOURCES += Compute.cpp \
    Accumulate.cpp \
    ZeroCrossings.cpp WindowedFourier.cpp ExtractMagnitude.cpp \
    ExtractPhase.cpp \
    Bark.cpp \
    SumRecord.cpp \
    MeanRecord.cpp \
    PeakGather.cpp \
    PeakTrack.cpp \
    CycleDelta.cpp
