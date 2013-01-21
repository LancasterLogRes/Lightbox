#include "Global.h"
#if LIGHTBOX_ANDROID
#include <jni.h>
#include <android_native_app_glue.h>
#elif LIGHTBOX_USE_XLIB
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elif LIGHTBOX_USE_SDL
#include <SDL/SDL.h>
#endif
#include <fstream>
#include <Common/Time.h>
#include "App.h"
#include "Display.h"
#include "AppEngine.h"
using namespace std;
using namespace Lightbox;

AppEngine* AppEngine::s_this = nullptr;

#if LIGHTBOX_ANDROID
AppEngine::AppEngine(struct android_app* _app):
	m_androidApp(_app)
{
	app_dummy();

	m_androidApp->userData = this;
	m_androidApp->onAppCmd = engine_handle_cmd;
	m_androidApp->onInputEvent = engine_handle_input;

	m_androidApp->activity->vm->AttachCurrentThread(&m_jni, nullptr);

	s_this = this;

	if (m_androidApp->savedState && m_app)
		m_app->setState(foreign_vector<uint8_t>((unsigned char*)m_androidApp->savedState, m_androidApp->savedStateSize));
}
#elif LIGHTBOX_USE_XLIB | LIGHTBOX_USE_SDL
AppEngine::AppEngine()
{
	s_this = this;
}
#endif

AppEngine::~AppEngine()
{
#if LIGHTBOX_ANDROID
	m_androidApp->activity->vm->DetachCurrentThread();
#endif
}

void AppEngine::startActivity(string const& _app, string const& _intent, function<void()> const& _onDone)
{
#if LIGHTBOX_ANDROID
	char const* args[] = { "/system/bin/am", "start", "-a", _intent.c_str(), _app.c_str() };
	cnote << "executing " << args[0] << args[1] << args[2] << args[3] << args[4];
	if (!fork())
	{
		execvp(args[0], (char**)args);
		exit(0);
	}
	m_onDoneLastActivity = _onDone;
	/*	jclass appClass = mEnv->GetObjectClass();
	jmethodID mid = mEnv->GetMethodID(appClass, "openURL", "(Ljava/lang/String;)V");
	mEnv->CallVoidMethod(appClassObj, mid, mEnv->NewStringUTF(url));
	appClassObj = m_androidApp->NewGlobalRef(thiz);*/
#else
	_onDone();
#endif

}

void AppEngine::setApp(App* _app)
{
	m_app = std::shared_ptr<App>(_app);
}

static const Time c_frameTime = FromSeconds<1>::value / 60;

void AppEngine::exec()
{
	m_app->go();
	m_app->sendInitialEvents();

#if LIGHTBOX_USE_XLIB
	gfxInit();
	gfxDraw();
	::Display* xDisplay = (::Display*)m_display->xDisplay();
	::Window xWindow = m_display->xWindow();
	XSelectInput(xDisplay, xWindow, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
#elif LIGHTBOX_USE_SDL
	gfxInit();
	gfxDraw();
#endif

	Time lastDraw = wallTime();
	Time lastTick = wallTime();
	for (bool carryOn = true; carryOn;)
	{
		if (wallTime() - lastTick >= FromSeconds<1>::value)
		{
			m_app->tick();
			lastTick = wallTime();
		}

		if ((m_display && (/*true || */m_display->isAnimating())) && wallTime() - lastDraw >= c_frameTime)
		{
			lastDraw = wallTime();
			gfxDraw();
			m_lastDrawTime = wallTime() - lastDraw;
			lastDraw = wallTime();
		}
		else
			usleep(30);

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		for (bool hadEvent = true; carryOn && hadEvent && (!m_display || !m_display->isAnimating() || wallTime() - lastDraw < FromSeconds<1>::value / 60);)
		{
#if LIGHTBOX_ANDROID
			// Read all pending events.
			int events;
			struct android_poll_source* source;
			hadEvent = ALooper_pollAll(/*true||*/(m_display && m_display->isAnimating()) ? 0 : 1000, NULL, &events, (void**)&source) > 0;
			if (hadEvent)
			{
				// Process this event.
				if (source)
					source->process(m_androidApp, source);

				if (m_androidApp->destroyRequested)
				{
					carryOn = false;
					break;
				}
			}
#elif LIGHTBOX_USE_XLIB
			for (; !(m_display && m_display->isAnimating()) && !XPending(xDisplay) && wallTime() - lastTick < FromSeconds<1>::value;)
				usleep(20);
			if (XPending(xDisplay) > 0)
			{
				XEvent event;
				XNextEvent(xDisplay, &event);

				switch (event.type)
				{
				case ButtonPress:
					m_app->motionEvent(0, iCoord(event.xbutton.x, event.xbutton.y), 1);
					break;
				case ButtonRelease:
					m_app->motionEvent(0, iCoord(event.xbutton.x, event.xbutton.y), -1);
					break;
				case MotionNotify:
					m_app->motionEvent(0, iCoord(event.xmotion.x, event.xmotion.y), 0);
					break;
				case DestroyNotify:
					carryOn = false;
					break;
				case Expose:
					m_display->repaint();
					break;
				default:;
				}
				hadEvent = true;
			}
			else
				hadEvent = false;
#elif LIGHTBOX_USE_SDL
			SDL_Event ev;
			for (hadEvent = SDL_PollEvent(&ev); !(m_display && m_display->isAnimating()) && !hadEvent && wallTime() - lastTick < FromSeconds<1>::value; hadEvent = SDL_PollEvent(&ev))
				usleep(20);
			if (hadEvent)
				switch (ev.type)
				{
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				{
					SDL_MouseButtonEvent& mev = (SDL_MouseButtonEvent&)ev;
					m_app->motionEvent(0, iCoord(mev.x, mev.y), ev.type == SDL_MOUSEBUTTONDOWN ? 1 : -1);
					break;
				}
				case SDL_MOUSEMOTION:
				{
					SDL_MouseMotionEvent& mev = (SDL_MouseMotionEvent&)ev;
					m_app->motionEvent(0, iCoord(mev.x, mev.y), ev.type == 0);
					break;
				}
				case SDL_QUIT:
					carryOn = false;
					break;
				case SDL_VIDEOEXPOSE:
					m_display->repaint();
					break;
				default:;
				}
#endif
		}
	}
	m_display.reset();
}

void AppEngine::gfxInit()
{
	m_display = make_shared<Display>();

	if (m_app)
		m_app->initGraphics(*m_display);

}

void AppEngine::gfxDraw()
{
	if (!m_display)
		return;

	bool stillDirty = false;
	if (m_app)
		stillDirty = !m_app->drawGraphics();

	m_display->update();
	if (stillDirty)
		m_display->repaint();
}

void AppEngine::gfxFini()
{
	if (m_app)
		m_app->finiGraphics(*m_display);

	m_display.reset();
}

#if LIGHTBOX_ANDROID
int32_t AppEngine::engine_handle_input(struct android_app* app, AInputEvent* event)
{
	return ((AppEngine*)app->userData)->handleInput(event);
}

int32_t AppEngine::handleInput(AInputEvent* _event)
{
	bool ret = false;
	if (AInputEvent_getType(_event) == AINPUT_EVENT_TYPE_MOTION)
	{
		int32_t action = AMotionEvent_getAction(_event);
		int flags = action & AMOTION_EVENT_ACTION_MASK;
		int index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		int id = AMotionEvent_getPointerId(_event, index);

		iCoord c(AMotionEvent_getX(_event, id), AMotionEvent_getY(_event, id));
		switch (flags)
		{
		case AMOTION_EVENT_ACTION_POINTER_UP: case AMOTION_EVENT_ACTION_UP:
			if (id >= 0 && id < 5)
			{
				cnote << "(" << id << "up" << m_pointerState[id] << ")";
				ret = m_app->motionEvent(id, m_pointerState[id], -1);
				m_pointerState[id] = iCoord(-1, -1);
			}
			break;
		case AMOTION_EVENT_ACTION_POINTER_DOWN: case AMOTION_EVENT_ACTION_DOWN:
			if (id >= 0 && id < 5)
			{
				ret = m_app->motionEvent(id, c, 1);
				cnote << "(" << id << "down" << c << ")";
				m_pointerState[id] = c;
			}
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			for (index = 0; index < 5; ++index)
			{
				id = AMotionEvent_getPointerId(_event, index);
				if (id >= 0 && id < 5)
				{
					c = iCoord(AMotionEvent_getX(_event, index), AMotionEvent_getY(_event, index));
					if (c != m_pointerState[index] && c.x() + c.y() > 4 && c.x() + c.y() < 2000 && (m_pointerState[index] - c).length() < 150)	//arbitrary limits to reduce blips on shitty touchscreens.
					{
						cnote << "(" << id << "[" << index << "] move" << m_pointerState[id] << "->" << c << ")";
						break;
					}
				}
			}
			if (index < 5 && id >= 0 && id < 5)
			{
				m_pointerState[id] = c;
				ret = m_app->motionEvent(id, c, 0);
			}
			break;
		default:;
		}
	}
	return ret ? 1 : 0;
}

void AppEngine::engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	((AppEngine*)app->userData)->handleCommand(cmd);
}

void AppEngine::handleCommand(int32_t _cmd)
{
	cnote << "Got command" << _cmd;
	switch (_cmd)
	{
	case APP_CMD_SAVE_STATE:
		if (m_app)
		{
			auto s = m_app->state();
			m_androidApp->savedState = malloc(s.size());
			memcpy(m_androidApp->savedState, s.data(), s.size());
			m_androidApp->savedStateSize = s.size();
		}
		break;
	case APP_CMD_INIT_WINDOW:
		if (m_androidApp->window)
		{
			gfxInit();

			if (m_onDoneLastActivity)
				m_onDoneLastActivity();
			m_onDoneLastActivity = nullptr;

			gfxDraw();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		gfxFini();
		break;
	case APP_CMD_LOST_FOCUS:
		gfxDraw();
		break;
	default:;
	}
}

struct AssetCloser
{
	~AssetCloser()
	{
		AAsset_close(m_id);
	}
	AAsset* m_id;
};

std::function<void(uint8_t*, size_t)> Lightbox::assetReader(std::string const& _filename)
{
	auto a = make_shared<AssetCloser>();
	if (a->m_id = AAssetManager_open(AppEngine::get()->androidApp()->activity->assetManager, _filename.c_str(), AASSET_MODE_UNKNOWN))
		return [=](uint8_t* d, size_t s)
		{
			AAsset_read(a->m_id, d, s);
		};
	else
	{
		cwarn << "Couldn't find asset" << _filename;
		return nullptr;
	}
}
#elif !defined(LIGHTBOX_CROSS)
std::function<void(uint8_t*, size_t)> Lightbox::assetReader(std::string const& _filename)
{
	auto a = make_shared<ifstream>();
	a->open(_filename);
	if (a->is_open())
		return [=](uint8_t* d, size_t s)
		{
			a->read((char*)d, s);
		};
	else
	{
		cwarn << "Couldn't find asset" << _filename;
		return nullptr;
	}
}
#endif

std::function<void(uint8_t*, size_t)> Lightbox::resReader(uint8_t const* _data)
{
	auto offset = make_shared<size_t>(0);
	return [=](uint8_t* d, size_t s)
	{
		memcpy(d, _data + *offset, s);
		*offset += s;
	};
}
