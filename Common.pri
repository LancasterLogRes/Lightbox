DESTDIR = $$OUT_PWD/../built

CONFIG += no_include_pwd
CONFIG -= uic
DEFINES += "LIGHTBOX_TARGET_NAME=$$TARGET"

android: CONFIG += cross
pi: CONFIG += cross
!pi: !android {
    cross: CONFIG += amd
    CONFIG += x86
}

pi|amd: CONFIG += force_static
!cross: CONFIG += force_shared

message($$CONFIG)

CONFIG(release, debug|release) {
	CONFIG += release ndebug
	CONFIG -= debug
	profile: QMAKE_CXXFLAGS += -g3
	!profile: QMAKE_CXXFLAGS += -g0 -fomit-frame-pointer -Wl,-strip-all
	QMAKE_CXXFLAGS += -DRELEASE -O2
	system (echo "Release build")
}

CONFIG(debug, debug|release) {
	CONFIG -= release ndebug
	QMAKE_CXXFLAGS += -DDEBUG -g3 -fno-inline -O0 -Wall
	!win32: QMAKE_CXXFLAGS += -fPIC
	system (echo "Debug build")
}

profile {
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}

pi {
        TP = $$IN_PWD/../thirdparty-pi
}
android {
        TP = $$IN_PWD/../thirdparty-android
}
x86 {
        TP = $$IN_PWD/../thirdparty-x86
}

INCLUDEPATH += $${OUT_PWD}
resource_compiler.output = $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.c $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.h
resource_compiler.input = RES
resource_compiler.commands = echo ${QMAKE_FILE_IN} ${QMAKE_FILE_IN_BASE} && rm -f $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.c $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.h && for i in `cat ${QMAKE_FILE_IN}`; do cd $${_PRO_FILE_PWD_} && xxd -i \$\$i >> $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.c; echo \"extern unsigned int \$\$(echo \$\$i | sed s/\\\\./_/)_len;\" >> $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.h; echo \"extern unsigned char \$\$(echo \$\$i | sed s/\\\\./_/)[];\" >> $${OUT_PWD}/${QMAKE_FILE_IN_BASE}.h; done
resource_compiler.variable_out = SOURCES
resource_compiler.depend_command = cat ${QMAKE_FILE_IN}
resource_compiler.CONFIG += target_predeps
resource_compiler.name = RESOURCE_COMPILER
QMAKE_EXTRA_COMPILERS += resource_compiler

QMAKE_CXXFLAGS += -ffast-math -pipe -fexceptions
mac: QMAKE_CXXFLAGS +=  -std=c++11
!mac: QMAKE_CXXFLAGS +=  -std=c++0x
cross {
    android {
        NDK_PATH = /home/gav/Projects/lightbox/Android/android-ndk-r8b
        SDK_PATH = /home/gav/Projects/lightbox/Android/android-sdk-linux

        CHAINPATH = "$$NDK_PATH/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin"
        PLATFORM = "arm-linux-androideabi"

        SYSTEM_FLAGS = -DNDEBUG -fpic -ffunction-sections -funwind-tables -fstack-protector -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack
        SYSTEM_INCLUDES = "-I$$NDK_PATH/sources/android/native_app_glue" "-I$$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/include" "-I$$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/include" "-I$$NDK_PATH/platforms/android-14/arch-arm/usr/include -I../../Android/Boost/boost_1_49_0"

        QMAKE_CC		= "$$CHAINPATH/$$PLATFORM-gcc"
        QMAKE_CXX               = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_LINK              = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_LINK_SHLIB        = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_AR                = "$$CHAINPATH/$$PLATFORM-ar crs"
        QMAKE_OBJCOPY           = "$$CHAINPATH/$$PLATFORM-objcopy"
        QMAKE_STRIP             = "$$CHAINPATH/$$PLATFORM-strip"

        QMAKE_CFLAGS		= $$SYSTEM_FLAGS $$SYSTEM_INCLUDES
        QMAKE_CXXFLAGS		= $$SYSTEM_FLAGS -std=gnu++0x -frtti $$SYSTEM_INCLUDES

        CONFIG -= qt

        QMAKE_LFLAGS_SHLIB = -shared --sysroot=$$NDK_PATH/platforms/android-14/arch-arm -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -L$$NDK_PATH/platforms/android-14/arch-arm/usr/lib
        LIBS += -L$$TP/libpng -L$$TP/libzip
        INCLUDEPATH += $$TP
        LIBSTL = "$$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/libgnustl_static.a"

        contains(TEMPLATE, lib) {
            CONFIG += static
        }
        contains(TEMPLATE, app) {
            SOURCES += "$$NDK_PATH/sources/android/native_app_glue/android_native_app_glue.c"
            TEMPLATE = lib
            CONFIG += force_shared
            QMAKE_POST_LINK = mkdir -p '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
                cp '"$${DESTDIR}/lib$${TARGET}.so"' '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
                sed '\'s/"android.app.lib_name" android:value=""/"android.app.lib_name" android:value="$$TARGET"/\'' '$${ANDROID_MANIFEST}' > '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' &&\
                mkdir -p '"$${OBJECTS_DIR}wrap/res/values"' &&\
                echo '\'<?xml version="1.0" encoding="utf-8"?><resources><string name="app_name">Mark2</string></resources>\'' > '$${OBJECTS_DIR}wrap/res/values/strings.xml' &&\
                mkdir -p '"$${OBJECTS_DIR}wrap/assets"' &&\
                for i in $${ANDROID_ASSETS}; do ln -s \$\$i' '"$${OBJECTS_DIR}wrap/assets"'; done &&\
                $${SDK_PATH}/tools/android update project -p '$${OBJECTS_DIR}wrap' -t android-15 -n $$TARGET &&\
                ant debug -buildfile '$${OBJECTS_DIR}wrap/build.xml' &&\
                mv '$${OBJECTS_DIR}wrap/bin/$$TARGET-debug.apk' '$${DESTDIR}/$${TARGET}.apk' &&\
                rm -rf '$${OBJECTS_DIR}wrap' &&\
                echo DONE.
            QMAKE_CLEAN = '$${DESTDIR}$${TARGET}.apk'
        }

        message($$QMAKE_CXX)

        DEFINES += LIGHTBOX_ANDROID
    }
    pi {
        PI_TOOLS = /home/gav/Projects/lightbox/RPi/x-tools/arm-unknown-linux-gnueabi/bin

        # modified straight out of arm-none-linux-gnueabi's qmakespec
        # /usr/share/qt4/mkspecs/qws/linux-arm-gnueabi-g++
        QMAKE_CC                = $$PI_TOOLS/arm-unknown-linux-gnueabi-gcc
        QMAKE_CXX               = $$PI_TOOLS/arm-unknown-linux-gnueabi-g++
        QMAKE_LINK              = $$PI_TOOLS/arm-unknown-linux-gnueabi-g++
        QMAKE_LINK_SHLIB        = $$PI_TOOLS/arm-unknown-linux-gnueabi-g++
        QMAKE_AR                = $$PI_TOOLS/arm-unknown-linux-gnueabi-ar cqs
        QMAKE_OBJCOPY           = $$PI_TOOLS/arm-unknown-linux-gnueabi-objcopy
        QMAKE_STRIP             = $$PI_TOOLS/arm-unknown-linux-gnueabi-strip

        QMAKE_CXXFLAGS += -O2 -pipe -march=armv6zk -mfpu=vfp -mfloat-abi=hard -mcpu=arm1176jzf-s

        DEFINES += LIGHTBOX_PI
    }
    x86 {
        QMAKE_CXXFLAGS += -march=amdfam10 -O2 -pipe -mno-3dnow -mcx16 -mpopcnt -msse3 -msse4a -mmmx
        DEFINES += LIGHTBOX_X86
    }
    DEFINES += LIGHTBOX_CROSS
}
!cross {
    QMAKE_CXXFLAGS += -march=native
    DEFINES += LIGHTBOX_NATIVE
}

QMAKE_CXXFLAGS_WARN_ON += -Wno-parentheses

INCLUDEPATH += $$IN_PWD
# For macports
mac: LIBS += -L/opt/local/lib

win32 {
	# Dependent on your configuration.
	BOOST = C:/boost_1_50_0
	FFTW = C:/Lightbox/fftw
	PORTAUDIO = C:/Lightbox/portaudio
	GLEW = C:/Lightbox/glew-1.8.0
	RESAMPLE = C:/Lightbox/libresample-0.1.3
	SNDFILE = C:/Lightbox/libsndfile
	LIBS += -L$$BOOST/stage/lib -L$$FFTW -L$$PORTAUDIO/lib/.libs -L$$GLEW/lib -L$$RESAMPLE -L$$SNDFILE/lib
	INCLUDEPATH += $$BOOST $$FFTW $$PORTAUDIO/include $$GLEW/include $$RESAMPLE/include $$SNDFILE/include
        FFTW3_LIBS = -lfftw3f-3
        SNDFILE_LIBS = $$SNDFILE/lib/libsndfile-1.lib
        GL_LIBS += -lOpenGL32 -lGLU32 -lGLEW32
}
!win32 {
        FFTW3_LIBS = -lfftw3f
        SNDFILE_LIBS = -lsndfile
        GL_LIBS += -lGL -lGLU -lGLEW
}

android: CONFIG += use_egl use_gles2
#x86: CONFIG += use_xlib use_egl use_gles2
x86: CONFIG += use_sdl use_gl

use_egl {
    DEFINES += LIGHTBOX_USE_EGL=1
    GFX_LIBS += -lEGL
}
use_xlib {
    DEFINES += LIGHTBOX_USE_XLIB=1
    GFX_LIBS += -lX11
}
use_sdl {
    DEFINES += LIGHTBOX_USE_SDL=1
    GFX_LIBS += -lSDL
}
use_gl {
    DEFINES += LIGHTBOX_USE_GL=1
    GFX_LIBS += -lGL
}
use_gles2 {
    DEFINES += LIGHTBOX_USE_GLES2=1
    GFX_LIBS += -lGLESv2
}

LIBS += -L$$DESTDIR -Wl,-rpath,$$DESTDIR
DEPENDPATH = $INCLUDEPATH

force_shared {
        CONFIG -= create_prl link_prl static
        CONFIG += shared dll dylib
        DEFINES += LIGHTBOX_SHARED_LIBRARY
        system (echo "Shared build")
}

force_static {
        CONFIG += create_prl link_prl static
        CONFIG -= shared dll dylib
        DEFINES += LIGHTBOX_STATIC_LIBRARY
        LIBS += -static
        system (echo "Static build")
}

isEmpty(LIGHTBOX_ROOT_PROJECT): LIGHTBOX_ROOT_PROJECT = Lightbox

message ("$$LIGHTBOX_ROOT_PROJECT <- $$LIGHTBOX_USES_PROJECTS")

defineReplace(ProjectPath) {
    To = $$1
    dd = $$DESTDIR
    dd ~= s:/[^/]*(/\\.)*/\\.\\.::
    dd ~= s/$$LIGHTBOX_ROOT_PROJECT/$$To/
    return($$dd)
}

defineReplace(IncludePath) {
    To = $$1
    dd = $$IN_PWD
    dd ~= s/Lightbox/$$To/
    return($$dd)
}

for(p, LIGHTBOX_USES_PROJECTS) {
    pp = $$ProjectPath($$p)
    LIBS += -L$$pp -Wl,-rpath,$$pp
    INCLUDEPATH += $$IncludePath($$p)
}

win32: DESTDIR = $$ProjectPath(Lightbox)
INCLUDEPATH += $$IncludePath($$LIGHTBOX_ROOT_PROJECT)
DEPENDPATH = $INCLUDEPATH

