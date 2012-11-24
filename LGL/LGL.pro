CONFIG -= qt
TARGET = LGL
TEMPLATE = lib
include ( ../Common.pri )
LIBS += -lEGL -lGLESv2 -lCommon -lNumeric

SOURCES += ProgramFace.cpp ShaderFace.cpp Uniform.cpp \
    Global.cpp \
    Attrib.cpp
HEADERS += ProgramFace.h ShaderFace.h Program.h \
    Shader.h Uniform.h \
    Global.h \
    All.h \
    ../LGL.h \
    Attrib.h
