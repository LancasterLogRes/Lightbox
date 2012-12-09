CONFIG -= qt
TARGET = LGL
TEMPLATE = lib
include ( ../Common.pri )
LIBS += $$GFX_LIBS -lCommon -lNumeric -lpng -lz

SOURCES += ProgramFace.cpp Program.cpp ShaderFace.cpp Uniform.cpp \
    Global.cpp \
    Attrib.cpp \
    Texture2D.cpp \
    TextureFace2D.cpp \
    BufferFace.cpp Buffer.cpp \
    Metrics.cpp
HEADERS += ProgramFace.h ShaderFace.h Program.h \
    Shader.h Uniform.h \
    Global.h \
    All.h \
    ../LGL.h \
    Attrib.h \
    Texture2D.h \
    TextureFace2D.h \
    BufferFace.h Buffer.h \
    GL.h \
    GLMap.h \
    Metrics.h

RES += Shaders.resources

OTHER_FILES += \
    Shaders.resources \
    Font.frag \
    Font.vert
