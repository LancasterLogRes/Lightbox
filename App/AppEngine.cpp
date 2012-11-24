#include <jni.h>
#include <android_native_app_glue.h>
#include "App.h"
#include "Display.h"
#include "AppEngine.h"
using namespace std;
using namespace Lightbox;

AppEngine* AppEngine::s_this = nullptr;

AppEngine::AppEngine(struct android_app* _app): m_androidApp(_app)
{
	app_dummy();

	m_androidApp->userData = this;
	m_androidApp->onAppCmd = engine_handle_cmd;
	m_androidApp->onInputEvent = engine_handle_input;

	s_this = this;

	if (m_androidApp->savedState && m_app)
		m_app->setState(foreign_vector<uint8_t>((unsigned char*)m_androidApp->savedState, m_androidApp->savedStateSize));
}

AppEngine::~AppEngine()
{
}

void AppEngine::setApp(App* _app)
{
	m_app = std::shared_ptr<App>(_app);
}

void AppEngine::exec()
{
	for (bool carryOn = true; carryOn;)
	{
		if (m_display && m_display->isAnimating())
			gfxDraw();

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
		m_app->drawGraphics(*m_display);

	m_display->update();
}

void AppEngine::gfxFini()
{
	m_display.reset();
}

int32_t AppEngine::engine_handle_input(struct android_app* app, AInputEvent* event)
{
	return ((AppEngine*)app->userData)->handleInput(event);
}

int32_t AppEngine::handleInput(AInputEvent* _event)
{
	if (AInputEvent_getType(_event) == AINPUT_EVENT_TYPE_MOTION && m_app->motionEvent(AMotionEvent_getX(_event, 0), AMotionEvent_getY(_event, 0), _event))
		return 1;
	return 0;
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
