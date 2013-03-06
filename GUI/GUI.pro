TEMPLATE = lib
include ( ../Common.pri )

LIBS += $$GFX_LIBS -lpng -lz
RES += Shaders.glsl HueWheel.glsl LightBar.glsl ubuntu_r.ttf ubuntu_b.ttf

SOURCES += \
    View.cpp \
    BasicButton.cpp \
    GUIApp.cpp \
    Joint.cpp \
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
    ProgressBar.cpp \
    FontManager.cpp \
	BakedFont.cpp \
    Slate.cpp \
    RenderToTextureSlate.cpp \
    Slider.cpp \
    ListView.cpp \
	MarkdownView.cpp \
DropListView.cpp \
SensitiveView.cpp \
	ListModelUser.cpp

HEADERS += \
    View.h \
    BasicButton.h \
    Global.h \
    GUIApp.h \
    All.h \
    Joint.h \
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
    ProgressBar.h \
    FontManager.h \
    BakedFont.h \
    Slate.h \
    RenderToTextureSlate.h \
    Slider.h \
    ListView.h \
	MarkdownView.h \
	Adaptors.h \
DropListView.h \
SensitiveView.h \
	ListModel.h \
	ListModelUser.h

OTHER_FILES += \
    Shaders.glsl \
    HueWheel.glsl \
    LightBar.glsl
