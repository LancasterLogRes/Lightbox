#include <sstream>
#include <EGL/egl.h>
#include <Common/Global.h>
#include <android_native_app_glue.h>
#include "AppEngine.h"
#include "Display.h"
using namespace std;
using namespace Lightbox;

Display::Display()
{
	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(m_display, 0, 0);

	EGLint format;
	EGLint numConfigs;
	EGLConfig config;

	EGLint const attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
	eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(AppEngine::get()->androidApp()->window, 0, 0, format);
	m_surface = eglCreateWindowSurface(m_display, config, AppEngine::get()->androidApp()->window, NULL);

	int attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	m_context = eglCreateContext(m_display, config, NULL, attrib_list);

	if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
	{
		cwarn << "Unable to eglMakeCurrent";
	}

	EGLint w;
	EGLint h;
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &w);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &h);
	m_width = w;
	m_height = h;
}

Display::~Display()
{
	if (m_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT)
			eglDestroyContext(m_display, m_context);
		if (m_surface != EGL_NO_SURFACE)
			eglDestroySurface(m_display, m_surface);
		eglTerminate(m_display);
	}
}

void Display::update()
{
	eglSwapBuffers(m_display, m_surface);
	m_animating = false;
}
