ANDROID_MANIFEST = $$PWD/AndroidManifest.xml
include (../Common.pri)

SOURCES += main.cpp \
	MainView.cpp \
	TestGUIApp.cpp

LIBS += $$GFX_LIBS -lpng -lz
android: LIBS += $$LIBSTL -llog -landroid

INCLUDEPATH += $${DESTDIR}
resource_compiler.CONFIG += target_predeps

RES += #Shaders.resources
OTHER_FILES += Shaders.resources AndroidManifest.xml

HEADERS += \
    MainView.h \
    TestGUIApp.h

# TODO: make it work for debug/release
QMAKE_EXTRA_TARGETS += deploy
deploy.depends = $$RELATIVE_DESTDIR/$(TARGET)
deploy.commands = $$ANDROID_SDK_PATH/platform-tools/adb install -r $$DESTDIR/TestGUI.apk && $$ANDROID_SDK_PATH/platform-tools/adb shell am start -n com.lancasterlogicresponse.wrapper/.NativeActivity
