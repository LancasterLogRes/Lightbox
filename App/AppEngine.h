#pragma once

#include <functional>
#include <memory>
#include <array>
#include <Numeric/Coord.h>
#include <Common/Global.h>
#include <Common/Time.h>
#include "Global.h"

#if LIGHTBOX_ANDROID
struct android_app;
struct AInputEvent;
#elif !defined(LIGHTBOX_CROSS)
#endif

struct _JNIEnv;
typedef _JNIEnv JNIEnv;

namespace Lightbox
{

class Display;
class App;

class AppEngine
{
public:
#if LIGHTBOX_ANDROID
	AppEngine(struct android_app* _app);

	struct android_app* androidApp() { return m_androidApp; }
#elif !defined(LIGHTBOX_CROSS)
	AppEngine();
#endif

	~AppEngine();

	App* app() const { return &*m_app; }
	void setApp(App* _app);
	Display& display() { return *m_display; }

	void startActivity(std::string const& _application, std::string const& _intent, std::function<void()> const& _onDone);

	Time lastDrawTime() const { return m_lastDrawTime; }

	void exec();

	static AppEngine* get() { assert(s_this); return s_this; }

private:
	AppEngine(AppEngine const&) = delete;
	AppEngine& operator=(AppEngine const&) = delete;

	void gfxInit();
	void gfxDraw();
	void gfxFini();

	std::shared_ptr<Display> m_display;
	std::shared_ptr<App> m_app;

	Time m_lastDrawTime;

#if LIGHTBOX_ANDROID
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event);
	int32_t handleInput(AInputEvent* _event);
	static void engine_handle_cmd(struct android_app* app, int32_t cmd);
	void handleCommand(int32_t _cmd);

	std::array<iCoord, 5> m_pointerState;

	struct android_app* m_androidApp;	///< The Android app object. Always valid.
	JNIEnv* m_jni;

	std::function<void()> m_onDoneLastActivity;
#elif !defined(LIGHTBOX_CROSS)
#endif

	static AppEngine* s_this;
};

std::function<void(uint8_t*, size_t)> resReader(uint8_t const* _data);
std::function<void(uint8_t*, size_t)> assetReader(std::string const& _filename);

}
