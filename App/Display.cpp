#if LIGHTBOX_CROSSCOMPILATION_ANDROID
#include <android_native_app_glue.h>
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include <sstream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <Common/Global.h>
#include "AppEngine.h"
#include "Display.h"
using namespace std;
using namespace Lightbox;

#define EGL_CHECK(X) if (!(X)) { cwarn << "FATAL:" << #X; exit(-1); } else void(0)

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
#endif

Lightbox::Display::Display()
{
#if LIGHTBOX_CROSSCOMPILATION_ANDROID
	EGL_CHECK(m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY));
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
	char* displayName = nullptr;
	::Display* xDisplay = XOpenDisplay(displayName);
	EGL_CHECK(m_display = eglGetDisplay(xDisplay));
#endif

	EGL_CHECK(eglInitialize(m_display, 0, 0));

	cnote << "EGL Version" << eglQueryString(m_display, EGL_VERSION);
	cnote << "EGL Vendor" << eglQueryString(m_display, EGL_VENDOR);
	cnote << "EGL Extensions" << eglQueryString(m_display, EGL_EXTENSIONS);
	cnote << "EGL Client APIs" << eglQueryString(m_display, EGL_CLIENT_APIS);

	int contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

	EGLint format;
	EGLint numConfigs;
	EGLConfig config;

#if LIGHTBOX_CROSSCOMPILATION_ANDROID

	EGLint const attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
	EGL_CHECK(eglChooseConfig(m_display, attribs, &config, 1, &numConfigs));
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(AppEngine::get()->androidApp()->window, 0, 0, format);

	auto win = AppEngine::get()->androidApp()->window;

#elif !defined(LIGHTBOX_CROSSCOMPILATION)
	const char* name = "OpenGL ES 2.x";
	int x = 0;
	int y = 0;
	int width = 1024;
	int height = 736;

	static const EGLint attribs[] = { EGL_RED_SIZE, 1, EGL_GREEN_SIZE, 1, EGL_BLUE_SIZE, 1, EGL_DEPTH_SIZE, 1, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };

	Window root = RootWindow(xDisplay, DefaultScreen(xDisplay));

	EGL_CHECK(eglChooseConfig(m_display, attribs, &config, 1, &numConfigs));
	assert(config);
	assert(numConfigs > 0);

	EGL_CHECK(eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format));

	int numVisuals;
	XVisualInfo visTemplate;
	visTemplate.visualid = format;
	XVisualInfo* visInfo = XGetVisualInfo(xDisplay, VisualNoMask, &visTemplate, &numVisuals);
	EGL_CHECK(visInfo);

	XSetWindowAttributes attr;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(xDisplay, root, visInfo->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	Window win = XCreateWindow(xDisplay, root, 0, 0, width, height, 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);

	{
	   XSizeHints sizehints;
	   sizehints.x = x;
	   sizehints.y = y;
	   sizehints.width  = width;
	   sizehints.height = height;
	   sizehints.flags = USSize | USPosition;
	   XSetNormalHints(xDisplay, win, &sizehints);
	   XSetStandardProperties(xDisplay, win, name, name, None, (char **)NULL, 0, &sizehints);
	}
#endif

	eglBindAPI(EGL_OPENGL_ES_API);

	EGL_CHECK(m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttribs));
	{
	   EGLint val;
	   eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &val);
	   assert(val == 2);
	}

	EGL_CHECK(m_surface = eglCreateWindowSurface(m_display, config, win, nullptr));
	{
	   EGLint val;
	   eglQuerySurface(m_display, m_surface, EGL_WIDTH, &val);
	   assert(val == width);
	   eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &val);
	   assert(val == height);
	   assert(eglGetConfigAttrib(m_display, config, EGL_SURFACE_TYPE, &val));
	   assert(val & EGL_WINDOW_BIT);
	}

#if !defined(LIGHTBOX_CROSSCOMPILATION)
	XFree(visInfo);
	XMapWindow(xDisplay, win);
#endif

	EGL_CHECK(eglMakeCurrent(m_display, m_surface, m_surface, m_context));

	EGLint w;
	EGLint h;
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &w);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &h);
	m_width = w;
	m_height = h;

	glViewport(0, 0, m_width, m_height);
}

Lightbox::Display::~Display()
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

void Lightbox::Display::update()
{
	eglSwapBuffers(m_display, m_surface);
	m_animating = false;
}

