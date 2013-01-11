ANDROID_NDK_PATH = $$PWD/../Android/ndk
ANDROID_NDK_VERSION = 14
ANDROID_SDK_PATH = $$PWD/../Android/sdk
ANDROID_TOOLCHAIN_VERSION = 4.7
ANDROID_JDK_PATH = $$PWD/../Android/jdk
PI_TOOLCHAIN_PATH = $$PWD/../Pi/x-tools/bin/

CONFIG += no_include_pwd
CONFIG -= uic qt
DEFINES += "LIGHTBOX_TARGET_NAME=$$TARGET"

# Tidy up the debug/release flags.
CONFIG(release, debug|release): CONFIG -= debug
CONFIG(debug, debug|release): CONFIG -= release
# one of {android, pi, amd} can be specified for cross-compilation; if none given will default to native.
android|pi|amd: CONFIG += cross
!cross: CONFIG += native
amd|native: CONFIG += x86
android|pi: CONFIG += arm
unix:!mac: CONFIG += linux

# Calculated flags:
# cross | native (cross -> android | pi | amd)
# arm | x86 (arm -> android | pi ; x86 -> amd | native)
# native -> (win32 | mac | linux)

pi: ARCH = Pi
x86: ARCH = x86
native: ARCH = Native
android: ARCH = Android
debug: BUILD_TYPE = Debug
release: BUILD_TYPE = Release
profile: BUILD_TYPE = Profile

pi|amd: CONFIG += force_static
native: CONFIG += force_shared

LLR_PATH = $$section(IN_PWD, "/", 0, -2)
RELATIVE_DESTDIR = ../built

!contains(TEMPLATE, subdirs): CONFIG += outputs

PROJECT_NAME = $$section(IN_PWD, "/", -1, -1)
!outputs {
	SUBPROJECT_NAME = ""
	PROJECT_PATH = $$PROJECT_NAME
	TARGET = $$PROJECT_NAME
	DESTDIR = /tmp
}
outputs {
	SUBPROJECT_NAME = $$section(OUT_PWD, "/", -1, -1)
	PROJECT_PATH = $$PROJECT_NAME/$$SUBPROJECT_NAME
	TARGET = $$SUBPROJECT_NAME
	DESTDIR = $$LLR_PATH/$$ARCH/$$PROJECT_NAME-$$BUILD_TYPE/built
}

#OUT_PWD = $$LLR_PATH/$$ARCH/$$PROJECT_NAME-$$BUILD_TYPE/$$SUBPROJECT_NAME

message ( "TEMPLATE:" $$TEMPLATE "TARGET:" $$TARGET "PROJECT: $$PROJECT_NAME/$$SUBPROJECT_NAME" "CONFIG:" $$find(CONFIG, (outputs)|(pi)|(amd)|(android)|(x86)|(native)|(cross)) "PWD:" $$PWD "OUT_PWD:" $$OUT_PWD "IN_PWD:" $$IN_PWD "DESTDIR:" $$DESTDIR)

release {
	CONFIG += ndebug
	profile: QMAKE_CXXFLAGS += -g3
	!profile: QMAKE_CXXFLAGS += -g0 -fomit-frame-pointer -Wl,-strip-all
	QMAKE_CXXFLAGS += -DRELEASE -O2
}
debug {
	CONFIG -= ndebug
	QMAKE_CXXFLAGS += -DDEBUG -g3 -fno-inline -O0 -Wall
	!win32: QMAKE_CXXFLAGS += -fPIC
}
profile {
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg
}

pi {
        TP = $$IN_PWD/../Pi/Thirdparty
}
android {
        TP = $$IN_PWD/../Android/Thirdparty
}
x86 {
        TP = $$IN_PWD/../x86/Thirdparty
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

android {
    TOOLCHAIN_PATH = "$$ANDROID_NDK_PATH/toolchains/arm-linux-androideabi-$$ANDROID_TOOLCHAIN_VERSION/prebuilt/linux-x86/bin/"
    PLATFORM_OVERRIDE = "arm-linux-androideabi"

    SYSTEM_FLAGS = -march=armv5te -mtune=xscale -msoft-float -mthumb -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__ -DANDROID -Wa,--noexecstack -fstack-protector -ffunction-sections
    SYSTEM_INCLUDES = "-I$$ANDROID_NDK_PATH/sources/android/native_app_glue" "-I$$ANDROID_NDK_PATH/sources/cxx-stl/gnu-libstdc++/$$ANDROID_TOOLCHAIN_VERSION/include" "-I$$ANDROID_NDK_PATH/sources/cxx-stl/gnu-libstdc++/$$ANDROID_TOOLCHAIN_VERSION/libs/armeabi/include" "-I$$ANDROID_NDK_PATH/platforms/android-$$ANDROID_NDK_VERSION/arch-arm/usr/include -I$$TP/include"

    QMAKE_CFLAGS		+= $$SYSTEM_FLAGS $$SYSTEM_INCLUDES
    QMAKE_CXXFLAGS		+= $$SYSTEM_FLAGS $$SYSTEM_INCLUDES
    QMAKE_CXXFLAGS_RELEASE      += -DNDEBUG -Os -fomit-frame-pointer -fno-strict-aliasing -fpic -funwind-tables -finline-limit=64

    CONFIG -= qt

	QMAKE_LFLAGS_SHLIB = -shared --sysroot=$$ANDROID_NDK_PATH/platforms/android-$$ANDROID_NDK_VERSION/arch-arm -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -L$$ANDROID_NDK_PATH/platforms/android-$$ANDROID_NDK_VERSION/arch-arm/usr/lib
    LIBS += -L$$TP/libpng -L$$TP/libzip
    INCLUDEPATH += $$TP
    LIBSTL = "$$ANDROID_NDK_PATH/sources/cxx-stl/gnu-libstdc++/$$ANDROID_TOOLCHAIN_VERSION/libs/armeabi/libgnustl_static.a"

    contains(TEMPLATE, lib) {
        CONFIG += static
    }
    contains(TEMPLATE, app) {
        SOURCES += "$$ANDROID_NDK_PATH/sources/android/native_app_glue/android_native_app_glue.c"
        TEMPLATE = lib
        CONFIG += force_shared
        QMAKE_POST_LINK = mkdir -p '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
            cp '"$${DESTDIR}/lib$${TARGET}.so"' '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
            sed '\'s/"android.app.lib_name" android:value=""/"android.app.lib_name" android:value="$$TARGET"/\'' '$${ANDROID_MANIFEST}' > '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' &&\
            mkdir -p '"$${OBJECTS_DIR}wrap/res/values"' &&\
            mkdir -p '"$${OBJECTS_DIR}wrap/src/com/lancasterlogicresponse/wrapper"' &&
        !isEmpty(ANDROID_JAVA_SOURCES) {
            QMAKE_POST_LINK += cp '$${ANDROID_JAVA_SOURCES}' '"$${OBJECTS_DIR}wrap/src/com/lancasterlogicresponse/wrapper"' &&
        }
        isEmpty(ANDROID_JAVA_SOURCES) {
            QMAKE_POST_LINK += mv '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' '"$${OBJECTS_DIR}wrap/AndroidManifest.xml.tmp"' && \
                sed '\'s/<application android:label="@string/app_name">/<application android:label="@string/app_name" android:hasCode="false">/\'' '"$${OBJECTS_DIR}wrap/AndroidManifest.xml.tmp"' > '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' &&
        }
        QMAKE_POST_LINK += \
            echo '\'<?xml version="1.0" encoding="utf-8"?><resources><string name="app_name">$${TARGET}</string></resources>\'' > '$${OBJECTS_DIR}wrap/res/values/strings.xml' &&\
            mkdir -p '"$${OBJECTS_DIR}wrap/assets"' &&\
            for i in $${ANDROID_ASSETS}; do ln -s '\$\$i' '"$${OBJECTS_DIR}wrap/assets"'; done &&\
			$${ANDROID_SDK_PATH}/tools/android update project -p '$${OBJECTS_DIR}wrap' -t 1 -n $$TARGET &&\
			JAVA_HOME="$$ANDROID_JDK_PATH" ant debug -buildfile '$${OBJECTS_DIR}wrap/build.xml' &&\
            mv '$${OBJECTS_DIR}wrap/bin/$${TARGET}-debug.apk' '$${DESTDIR}/$${TARGET}.apk' &&\
            rm -rf '$${OBJECTS_DIR}wrap' &&\
            echo DONE.
        QMAKE_CLEAN = '$${DESTDIR}$${TARGET}.apk'
    }

    DEFINES += LIGHTBOX_ANDROID
    system( echo "$${LITERAL_HASH}include \"$$[QT_INSTALL_DATA]/mkspecs/linux-g++/qplatformdefs.h\"" > /tmp/qplatformdefs.h )
}
pi {
    # modified straight out of arm-none-linux-gnueabi's qmakespec
    # /usr/share/qt4/mkspecs/qws/linux-arm-gnueabi-g++
    TOOLCHAIN_PATH = "$$PI_TOOLCHAIN_PATH/"
    PLATFORM_OVERRIDE = arm-unknown-linux-gnueabi
    QMAKE_CXXFLAGS += -march=armv6zk -mfpu=vfp -mfloat-abi=hard -mcpu=arm1176jzf-s -I$$TP/include
    QMAKE_LFLAGS += -L$$TP/lib

    DEFINES += LIGHTBOX_PI
    system( echo "$${LITERAL_HASH}include \"$$[QT_INSTALL_DATA]/mkspecs/linux-g++/qplatformdefs.h\"" > /tmp/qplatformdefs.h )
}
amd {
    QMAKE_CXXFLAGS += -march=amdfam10 -pipe -mno-3dnow -mcx16 -mpopcnt -msse3 -msse4a -mmmx
    DEFINES += LIGHTBOX_X86
}
cross {
    DEFINES += LIGHTBOX_CROSS
}
native {
    QMAKE_CXXFLAGS += -march=native
    DEFINES += LIGHTBOX_NATIVE
}

!isEmpty(PLATFORM_OVERRIDE) {
    QMAKE_CC                = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-gcc"
    QMAKE_CXX               = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-g++"
    QMAKE_LINK              = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-g++"
    QMAKE_LINK_SHLIB        = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-g++"
    QMAKE_AR                = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-ar crs"
    QMAKE_OBJCOPY           = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-objcopy"
    QMAKE_STRIP             = "$$TOOLCHAIN_PATH$$PLATFORM_OVERRIDE-strip"
}

QMAKE_CXXFLAGS += -pipe -fexceptions -std=gnu++11 -frtti -ffast-math
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
