ANDROID_MANIFEST = $$PWD/AndroidManifest.xml
include (../Common.pri)

SOURCES += main.cpp \
	MainView.cpp \
	TestGUIApp.cpp

#android: LIBS += -L$${TP}/MonkVG/projects/MonkVG-Android/obj/local/armeabi -L$${TP}/MonkSVG/projects/MonkSVG-Android/obj/local/armeabi
android: LIBS += $${STP}/MonkSVG/projects/MonkSVG-Android/obj/local/armeabi/libMonkSVG.a $${STP}/MonkVG/projects/MonkVG-Android/obj/local/armeabi/libOpenVGU.a $${STP}/MonkVG/projects/MonkVG-Android/obj/local/armeabi/libOpenVG.a -lGLESv1_CM
!android: LIBS += -L$${STP}/MonkSVG/projects/MonkSVG-qmake -L$${STP}/MonkVG/projects/MonkVG-autotools/.libs -lMonkSVG -lOpenVGU -lOpenVG -lGLESv1_CM

LIBS += $$GFX_LIBS -lpng -lz
android: LIBS += $$LIBSTL -llog -landroid

INCLUDEPATH += $${DESTDIR} $${STP}/MonkVG/include $${STP}/MonkSVG/src

RES += Blur.glsl speed0.svg speed1.svg speed2.svg circle.svg

OTHER_FILES += AndroidManifest.xml \
	Blur.glsl speed0.svg speed1.svg speed2.svg circle.svg

HEADERS += \
    MainView.h \
    TestGUIApp.h

# TODO: make it work for debug/release
QMAKE_EXTRA_TARGETS += deploy
deploy.depends = $$RELATIVE_DESTDIR/$(TARGET)
deploy.commands = $$ANDROID_SDK_PATH/platform-tools/adb install -r $$DESTDIR/TestGUI.apk && $$ANDROID_SDK_PATH/platform-tools/adb shell am start -n com.lancasterlogicresponse.testgui/android.app.NativeActivity
