TEMPLATE = lib
include ( ../Common.pri )

LIBS += $$GFX_LIBS
!pi: LIBS += -lpng -lz

SOURCES += ProgramFace.cpp Program.cpp ShaderFace.cpp Uniform.cpp \
    Global.cpp \
    Attrib.cpp \
    Texture2D.cpp \
    TextureFace2D.cpp \
    BufferFace.cpp Buffer.cpp \
    Metrics.cpp \
	Framebuffer.cpp
!android: SOURCES += Collated.cpp

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
    Metrics.h \
    Framebuffer.h
