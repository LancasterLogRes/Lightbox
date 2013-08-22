# Lightbox/Common.pri
# Lightbox common project include file.
# This is a long and complex file most of which you don't need to know about.
# This top bit houses the configuration section which you do need to know about.

# Beginning of configuration:

# If you're on Windows, you'll need to set up the BOOST & FFTW project paths:
win32 {
	BOOST = C:/boost_1_50_0
	FFTW = C:/Lightbox/fftw
}

# If you're on Mac, you should to confirm the MacPorts library path is correct.
mac {
	MACPORTS_LIBS = /opt/local/lib
}

# Uncomment to build App framework/GUI stuff (wholly independent of Qt).
CONFIG += llr

# End of configurable part
##############################################################################

DEFINES += LIGHTBOX_VERSION=1.0.0 LIGHTBOX_BUILD_DATE="`date +'%y%m%d%H%M\'`"

ANDROID_NDK_PATH = $$PWD/../Android/ndk
ANDROID_NDK_VERSION = 14
ANDROID_SDK_PATH = $$PWD/../Android/sdk
ANDROID_TOOLCHAIN_VERSION = 4.7
ANDROID_JDK_PATH = $$PWD/../Android/jdk
PI_TOOLCHAIN_PATH = $$PWD/../Pi/x-tools/bin/

include (Lightbox.depends)

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
android: ARCH = Android
native: ARCH = Native
debug: BUILD_TYPE = Debug
release: BUILD_TYPE = Release
profile: BUILD_TYPE = Profile

amd: CONFIG += force_static
pi|native: CONFIG += force_shared

LLR_PATH = $$section(IN_PWD, "/", 0, -2)
RELATIVE_DESTDIR = ../built

!contains(TEMPLATE, subdirs): CONFIG += outputs

isEmpty(LIGHTBOX_ROOT_PROJECT): LIGHTBOX_ROOT_PROJECT = Lightbox
PROJECT_NAME = $$LIGHTBOX_ROOT_PROJECT
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

#message ( "TEMPLATE:" $$TEMPLATE "TARGET:" $$TARGET "PROJECT: $$PROJECT_NAME/$$SUBPROJECT_NAME" "CONFIG:" $$find(CONFIG, (outputs)|(pi)|(amd)|(android)|(x86)|(native)|(cross)) "PWD:" $$PWD "OUT_PWD:" $$OUT_PWD "IN_PWD:" $$IN_PWD "DESTDIR:" $$DESTDIR)

final {
	QMAKE_CXXFLAGS += -DFINAL
	DEFINES += LIGHTBOX_FINAL
	CONFIG += release
}
profile {
	QMAKE_CXXFLAGS += -g3 -pg -DPROFILE
	QMAKE_LFLAGS += -pg
	DEFINES += LIGHTBOX_PROFILE
	CONFIG += release
}
release {
	CONFIG += ndebug
	!profile: QMAKE_CXXFLAGS += -g0 -fomit-frame-pointer -Wl,-strip-all
	QMAKE_CXXFLAGS += -DRELEASE -O2
}
debug {
	CONFIG -= ndebug
	QMAKE_CXXFLAGS += -DDEBUG -g3 -fno-inline -O0 -Wall
	!win32: QMAKE_CXXFLAGS += -fPIC
}

STP = $$IN_PWD/../Thirdparty
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
		CONFIG += force_shared shared_from_static
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
				sed '\'s|<application android:label="@string/app_name">|<application android:label="@string/app_name" android:hasCode="false">|\'' '"$${OBJECTS_DIR}wrap/AndroidManifest.xml.tmp"' > '"$${OBJECTS_DIR}wrap/AndroidManifest.xml"' &&
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

	OBJCOPY_ARGS = -I binary -B arm -O elf32-littlearm

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

	OBJCOPY_ARGS = -I binary -B arm -O elf32-littlearm

    DEFINES += LIGHTBOX_PI
    system( echo "$${LITERAL_HASH}include \"$$[QT_INSTALL_DATA]/mkspecs/linux-g++/qplatformdefs.h\"" > /tmp/qplatformdefs.h )
}
x86 {
	OBJCOPY_ARGS = -B i386 -I binary -O elf64-x86-64
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

inline_resource_compiler1.output = ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}.obj ${QMAKE_FILE_IN_BASE}.h
inline_resource_compiler1.input = RES
inline_resource_compiler1.commands = OWD=`pwd` && cd '${QMAKE_FILE_IN_PATH}' && $${QMAKE_OBJCOPY} $${OBJCOPY_ARGS} ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT} \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}.obj
# &&\
#	echo '\'$${LITERAL_HASH}pragma once\'' > \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h &&\
#	echo "// Generated by qmake - DO NOT EDIT" >> \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h &&\
#	echo "'extern char _binary_${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_start;'" | sed s/\\\\./_/ >> \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h &&\
#	echo "'extern char _binary_${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_end;'" | sed s/\\\\./_/ >> \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h &&\
#	echo "'static unsigned int ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_len = &_binary_${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_end - &_binary_${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_start;'" | sed s/\\\\./_/g >> \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h &&\
#	echo "'static const unsigned char* ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT} = (const unsigned char*)&_binary_${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}_start;'" | sed s/\\\\./_/g >> \$\$OWD/${OBJECTS_DIR}/${QMAKE_FILE_IN_BASE}.h
inline_resource_compiler1.variable_out = OBJECTS
inline_resource_compiler1.depend_command =
inline_resource_compiler1.CONFIG += target_predeps
inline_resource_compiler1.name = RESOURCE_COMPILER
QMAKE_EXTRA_COMPILERS += inline_resource_compiler1

inline_resource_compiler2.output = res.cpp
inline_resource_compiler2.input = RES
inline_resource_compiler2.commands = \
	echo '\'$${LITERAL_HASH}include <Common/Global.h>\'' > ${QMAKE_FILE_OUT} &&\
	for i in ${QMAKE_FILE_IN}; do \
		f=`echo "\$\$i" | sed 's:.*/::g' | sed 's/[\\\\\\\\.-]/_/g'` &&\
		echo '\"extern uint8_t const _binary_\$\${f}_start;'\" | sed 's/\\\\./_/' >> ${QMAKE_FILE_OUT} &&\
		echo '\"extern uint8_t const _binary_\$\${f}_end;'\" | sed 's/\\\\./_/' >> ${QMAKE_FILE_OUT} ;\
	done ;\
	echo 'static bool s_registerResources = true' >> ${QMAKE_FILE_OUT} &&\
	for i in ${QMAKE_FILE_IN}; do \
		f=`echo "\$\$i" | sed 's:.*/::g'` &&\
		echo -n '\" && lb::Resources::add(\\\"\$\$f\\\", \"' >> ${QMAKE_FILE_OUT} &&\
		echo -n '\"lb::foreign(&_binary_\$\${f}_start, &_binary_\$\${f}_end\"' | sed 's/[\\\\.-]/_/g' >> ${QMAKE_FILE_OUT} &&\
		echo -n ' - ' >> ${QMAKE_FILE_OUT} &&\
		echo '\"&_binary_\$\${f}_start))\"' | sed 's/[\\\\.-]/_/g' >> ${QMAKE_FILE_OUT} ;\
	done ;\
	echo '\';\'' >> ${QMAKE_FILE_OUT}
inline_resource_compiler2.variable_out = SOURCES
inline_resource_compiler2.depend_command =
inline_resource_compiler2.CONFIG += combine
inline_resource_compiler2.name = RESOURCE_COMPILER
QMAKE_EXTRA_COMPILERS += inline_resource_compiler2

QMAKE_CXXFLAGS += -pipe -fexceptions -std=gnu++11 -frtti -ffast-math
QMAKE_CXXFLAGS_WARN_ON += -Wno-parentheses

INCLUDEPATH += $$IN_PWD

# For macports
mac: LIBS += -L$$MACPORTS_LIBS

win32 {
	INCLUDEPATH += $$BOOST $$FFTW
	LIBS += -L$$BOOST/stage/lib -L$$FFTW
	FFTW3_LIBS = -lfftw3f-3
	GL_LIBS += -lOpenGL32 -lGLU32
}
!win32 {
	FFTW3_LIBS = -lfftw3f
	SNDFILE_LIBS = -lsndfile
	GL_LIBS += -lGL -lGLU
}

pi {
	VCUL = $$TP/userland
	INCLUDEPATH += $$VCUL/host_applications/linux/libs/bcm_host/include $$VCUL/interface/vcos/pthreads $$VCUL/interface/vmcs_host/linux $$VCUL $$VCUL/interface/khronos/include
	GFX_LIBS += -L$$VCUL/build/../build/lib -shared -lbcm_host -lvcos -lvchiq_arm
}

pi: CONFIG += use_nwtpi use_gles2
android: CONFIG += use_egl use_gles2
x86: CONFIG += use_xlib use_egl use_gles2
#x86: CONFIG += use_sdl use_gl

use_egl {
	DEFINES += LIGHTBOX_USE_EGL=1
	GFX_LIBS += -lEGL
}
use_nwtpi {
	DEFINES += LIGHTBOX_USE_NWTPI=1
	NWTPI = $$TP/nwtpi-read-only
	INCLUDEPATH += $$NWTPI $$NWTPI/OEGL $$NWTPI/Native
	GFX_LIBS += -L$$NWTPI -shared -lnwtpi -lEGL
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
#        system (echo "Shared build")
}

force_static {
        CONFIG += create_prl link_prl static
        CONFIG -= shared dll dylib
        DEFINES += LIGHTBOX_STATIC_LIBRARY
        LIBS += -static
#        system (echo "Static build")
}

win32: LIBRARY_PREFIX =
win32: shared: LIBRARY_SUFFIX = .dll
win32: static: LIBRARY_SUFFIX = .lib
unix: LIBRARY_PREFIX = lib
mac: shared: LIBRARY_SUFFIX = .dylib
linux: shared: LIBRARY_SUFFIX = .so
unix: static: LIBRARY_SUFFIX = .a

#message ("$$LIGHTBOX_ROOT_PROJECT <- $$LIGHTBOX_USES_PROJECTS")

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

defineReplace(runique) {
	for(d, 1) {
		rdeps = $$d $$rdeps
	}
	rdeps = $$unique(rdeps)
	for(d, rdeps) {
		!contains(ret, $$d) {
			ret = $$d $$ret
		}
	}
	return($$ret)
}
# ten levels of recursion
levels = 0 1 2 3 4 5 6 7 8 9
outputs {
	deps = $$eval($${TARGET}.depends)
	for(dummy, levels) {
		for(d, deps) {
			deps += $$eval($${d}.depends)
		}
	}
	deps = $$runique($$deps)
	for(d, deps) {
		LIBS += -l$$d
		exists($${OUT_PWD}/$${d}): PRE_TARGETDEPS += $$d
	}
}

defineTest(tidy) {
	SD = $$SUBDIRS
	for(sd, SD) {
#		message ("Tidying $$sd (depends: " $$eval($${sd}.depends) ")" )
		ds = $$eval($${sd}.depends)
		for(d, ds) {
			!exists($${OUT_PWD}/$${d}) {
#				message ("Killing $${d}...")
				eval($${sd}.depends -= $$d)
			}
		}
		export($${sd}.depends)
#		message ("Tidyied $$sd (depends: " $$eval($${sd}.depends) ")" )
	}
}

shared_from_static | has_static_plugins: {
	QMAKE_LFLAGS += -Wl,--whole-archive -Wl,-zmuldefs
	LIBS += -Wl,--no-whole-archive
}

win32: DESTDIR = $$ProjectPath(Lightbox)
INCLUDEPATH += $$IncludePath($$LIGHTBOX_ROOT_PROJECT)
DEPENDPATH = $INCLUDEPATH

OTHER_FILES +=

#message ($$TARGET "deps:" $$deps)
