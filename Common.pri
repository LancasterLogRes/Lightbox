DESTDIR = $$OUT_PWD/../built

CONFIG += no_include_pwd
CONFIG -= uic
DEFINES += "LIGHTBOX_TARGET_NAME=$$TARGET"

android: CONFIG += crosscompilation
pi: CONFIG += crosscompilation
crosscompilation: !pi: !android: CONFIG += x86
!crosscompilation: CONFIG += x86

pi|x86: CONFIG += force_static
!crosscompilation: CONFIG += force_shared

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

QMAKE_CXXFLAGS += -ffast-math -pipe -fexceptions
mac: QMAKE_CXXFLAGS +=  -std=c++11
!mac: QMAKE_CXXFLAGS +=  -std=c++0x
crosscompilation {
    android {
        NDK_PATH = /home/gav/Projects/lightbox/Android/android-ndk-r8b
        SDK_PATH = /home/gav/Projects/lightbox/Android/android-sdk-linux

        CHAINPATH = "$$NDK_PATH/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin"
        PLATFORM = "arm-linux-androideabi"

        SYSTEM_FLAGS = '-DNDEBUG -DCAT_TARGET_NAME=$$TARGET' -fpic -ffunction-sections -funwind-tables -fstack-protector -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack
        SYSTEM_INCLUDES = "-I$$NDK_PATH/sources/android/native_app_glue" "-I$$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/include" "-I$$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/include" "-I$$NDK_PATH/platforms/android-14/arch-arm/usr/include -I../../Android/Boost/boost_1_49_0"

        QMAKE_CC		= "$$CHAINPATH/$$PLATFORM-gcc"
        QMAKE_CXX               = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_LINK              = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_LINK_SHLIB        = "$$CHAINPATH/$$PLATFORM-g++"
        QMAKE_AR                = "$$CHAINPATH/$$PLATFORM-ar crs"
        QMAKE_OBJCOPY           = "$$CHAINPATH/$$PLATFORM-objcopy"
        QMAKE_STRIP             = "$$CHAINPATH/$$PLATFORM-strip"

        QMAKE_CFLAGS		= $$SYSTEM_FLAGS $$SYSTEM_INCLUDES
        QMAKE_CXXFLAGS		= $$SYSTEM_FLAGS -std=gnu++0x $$SYSTEM_INCLUDES

        CONFIG -= qt

        QMAKE_LFLAGS_SHLIB = -shared --sysroot=$$NDK_PATH/platforms/android-14/arch-arm -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -L$$NDK_PATH/platforms/android-14/arch-arm/usr/lib
        LIBS += $$NDK_PATH/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/libgnustl_static.a -L$$TP/libpng -L$$TP/libzip
        INCLUDEPATH += $$TP

        contains(TEMPLATE, lib) {
            CONFIG += static
        }
        contains(TEMPLATE, app) {
            SOURCES += "$$NDK_PATH/sources/android/native_app_glue/android_native_app_glue.c"
            TEMPLATE = lib
            CONFIG += shared
            QMAKE_POST_LINK = mkdir -p '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
                cp '"$${DESTDIR}/lib$${TARGET}.so"' '"$${OBJECTS_DIR}wrap/libs/armeabi"' &&\
                sed '\'s/"android.app.lib_name" android:value=""/"android.app.lib_name" android:value="$$TARGET"/\'' '$$ANDROID_MANIFEST' > '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' &&\
                mkdir -p '"$${OBJECTS_DIR}wrap/res/values"' &&\
                echo '\'<?xml version="1.0" encoding="utf-8"?><resources><string name="app_name">Glow</string></resources>\'' > '$${OBJECTS_DIR}wrap/res/values/strings.xml' &&\
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

        DEFINES += LIGHTBOX_CROSSCOMPILATION_ANDROID
    }
    pi {
        # modified straight out of arm-none-linux-gnueabi's qmakespec
        # /usr/share/qt4/mkspecs/qws/linux-arm-gnueabi-g++
        QMAKE_CC                = arm-unknown-linux-gnueabi-gcc
        QMAKE_CXX               = arm-unknown-linux-gnueabi-g++
        QMAKE_LINK              = arm-unknown-linux-gnueabi-g++
        QMAKE_LINK_SHLIB        = arm-unknown-linux-gnueabi-g++
        QMAKE_AR                = arm-unknown-linux-gnueabi-ar cqs
        QMAKE_OBJCOPY           = arm-unknown-linux-gnueabi-objcopy
        QMAKE_STRIP             = arm-unknown-linux-gnueabi-strip

        QMAKE_CXXFLAGS += -O2 -pipe -march=armv6zk -mfpu=vfp -mfloat-abi=hard -mcpu=arm1176jzf-s

        DEFINES += LIGHTBOX_CROSSCOMPILATION_PI
    }
    x86 {
        QMAKE_CXXFLAGS += -march=amdfam10 -O2 -pipe -mno-3dnow -mcx16 -mpopcnt -msse3 -msse4a -mmmx
        DEFINES += LIGHTBOX_CROSSCOMPILATION_PC
    }
    DEFINES += LIGHTBOX_CROSSCOMPILATION
}
!crosscompilation {
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

LIBS += -L$$DESTDIR -Wl,-rpath,$$DESTDIR
DEPENDPATH = $INCLUDEPATH
