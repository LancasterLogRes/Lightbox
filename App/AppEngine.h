/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <memory>
#include <array>
#include <Numeric/Coord.h>
#include <Common/Global.h>
#include "Global.h"

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
struct android_app;
struct AInputEvent;
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#endif

namespace Lightbox
{

class Display;
class App;

class AppEngine
{
public:
#if LIGHTBOX_CROSSCOMPILATION_ANDROID
	AppEngine(struct android_app* _app);

	struct android_app* androidApp() { return m_androidApp; }
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
	AppEngine();
#endif

	~AppEngine();

	App* app() const { return &*m_app; }
	void setApp(App* _app);
	Display& display() { return *m_display; }

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

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event);
	int32_t handleInput(AInputEvent* _event);
	static void engine_handle_cmd(struct android_app* app, int32_t cmd);
	void handleCommand(int32_t _cmd);

	std::array<iCoord, 5> m_pointerState;

	struct android_app* m_androidApp;	///< The Android app object. Always valid.
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#endif

	static AppEngine* s_this;
};

std::function<void(uint8_t*, size_t)> resReader(uint8_t const* _data);
std::function<void(uint8_t*, size_t)> assetReader(std::string const& _filename);

}
