TEMPLATE = lib
include ( ../Common.pri )

PRE_TARGETDEPS += ../LGL ../App ../Numeric ../Common
LIBS += -lApp -lLGL $$GFX_LIBS -lNumeric -lCommon -lpng -lz
RES += Shaders.resources Fonts.resources
SOURCES += \
    View.cpp \
    BasicButton.cpp \
    GUIApp.cpp \
    Joint.cpp \
    FontFace.cpp \
    Font.cpp \
    ToggleButton.cpp \
    TextLabel.cpp \
    Event.cpp \
    Layout.cpp \
    Tabs.cpp \
    HuePicker.cpp \
    LightPicker.cpp \
    PatternPicker.cpp \
    DirectionPicker.cpp \
    Frame.cpp \
    Collated.cpp \
    ProgressBar.cpp

HEADERS += \
    View.h \
    BasicButton.h \
    Global.h \
    GUIApp.h \
    All.h \
    Joint.h \
    FontFace.h \
    Font.h \
    ToggleButton.h \
    TextLabel.h \
    Event.h \
    Layout.h \
    Tabs.h \
    HuePicker.h \
    LightPicker.h \
    PatternPicker.h \
    DirectionPicker.h \
    Frame.h \
    ProgressBar.h
OTHER_FILES += Shaders.resources View.frag View.vert \
    Fonts.resources \
    Flat.frag \
    Flat.vert \
    Font.frag \
    Font.vert \
    Texture.frag \
    Texture.vert \
    HueWheel.frag \
    HueWheel.vert \
    LightBar.vert
