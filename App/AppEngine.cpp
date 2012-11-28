#if LIGHTBOX_CROSSCOMPILATION_ANDROID
#include <jni.h>
#include <android_native_app_glue.h>
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#endif
#include <fstream>
#include "App.h"
#include "Display.h"
#include "AppEngine.h"
using namespace std;
using namespace Lightbox;

AppEngine* AppEngine::s_this = nullptr;

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
AppEngine::AppEngine(struct android_app* _app):m_androidApp(_app)
{
	app_dummy();

	m_androidApp->userData = this;
	m_androidApp->onAppCmd = engine_handle_cmd;
	m_androidApp->onInputEvent = engine_handle_input;

	s_this = this;

	if (m_androidApp->savedState && m_app)
		m_app->setState(foreign_vector<uint8_t>((unsigned char*)m_androidApp->savedState, m_androidApp->savedStateSize));
}
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
AppEngine::AppEngine()
{
	s_this = this;
}
#endif

AppEngine::~AppEngine()
{
}

void AppEngine::setApp(App* _app)
{
	m_app = std::shared_ptr<App>(_app);
}

void AppEngine::exec()
{
#if !defined(LIGHTBOX_CROSSCOMPILATION)
	gfxInit();
	gfxDraw();
#endif

	for (bool carryOn = true; carryOn;)
	{
		if (m_display && m_display->isAnimating())
			gfxDraw();
		else
			usleep(30);

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll((m_display && m_display->isAnimating()) ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
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
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#endif
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

	if (m_app)
		m_app->drawGraphics();

	m_display->update();
}

void AppEngine::gfxFini()
{
	m_display.reset();
}

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
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
				ret = m_app->motionEvent(id, m_pointerState[id], -1);
				m_pointerState[id] = iCoord(-1, -1);
			}
			break;
		case AMOTION_EVENT_ACTION_POINTER_DOWN: case AMOTION_EVENT_ACTION_DOWN:
			if (id >= 0 && id < 5)
			{
				ret = m_app->motionEvent(id, c, 1);
				m_pointerState[id] = c;
			}
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			for (int index = 0; index < 5; ++index)
			{
				id = AMotionEvent_getPointerId(_event, index);
				if (id >= 0 && id < 5)
				{
					c = iCoord(AMotionEvent_getX(_event, id), AMotionEvent_getY(_event, id));
					if (c != m_pointerState[id])
						break;
				}
			}
			if (id >= 0 && id < 5)
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
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
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
