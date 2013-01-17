#include "Global.h"
#if LIGHTBOX_ANDROID
#include <android_native_app_glue.h>
#endif
#if LIGHTBOX_USE_XLIB
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#if LIGHTBOX_USE_SDL
#include <SDL/SDL.h>
#endif
#if LIGHTBOX_USE_EGL
#include <EGL/egl.h>
#endif
#include <sstream>
#include <LGL/GL.h>
#include <Common/Global.h>
#include "AppEngine.h"
#include "Display.h"
using namespace std;
using namespace Lightbox;

#define EGL_CHECK(X) if (!(X)) { cwarn << "FATAL:" << #X; exit(-1); } else void(0)
#define SDL_CHECK(X) if (!(X)) { cwarn << "FATAL:" << #X << SDL_GetError(); SDL_Quit(); exit(-1); } else void(0)

#if LIGHTBOX_ANDROID
#elif !defined(LIGHTBOX_CROSS)
#endif

int c_defaultWidth = 800;
int c_defaultHeight = 480;

Lightbox::Display::Display()
{
#if LIGHTBOX_USE_EGL
#if LIGHTBOX_ANDROID
	EGL_CHECK(m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY));
#elif LIGHTBOX_USE_XLIB
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

	const EGLint attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };

	EGL_CHECK(eglChooseConfig(m_display, attribs, &config, 1, &numConfigs));
	assert(config);
	assert(numConfigs > 0);
	EGL_CHECK(eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format));

#if LIGHTBOX_ANDROID
	ANativeWindow_setBuffersGeometry(AppEngine::get()->androidApp()->window, 0, 0, format);
	auto win = AppEngine::get()->androidApp()->window;

#elif LIGHTBOX_USE_XLIB
	const char* name = "OpenGL";
	int x = 0;
	int y = 0;
	int width = c_defaultWidth;
	int height = c_defaultHeight;

	Window root = RootWindow(xDisplay, DefaultScreen(xDisplay));
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
	   assert(val == (int)m_width);
	   eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &val);
	   assert(val == (int)m_height);
	   assert(eglGetConfigAttrib(m_display, config, EGL_SURFACE_TYPE, &val));
	   assert(val & EGL_WINDOW_BIT);
	}

#if LIGHTBOX_USE_XLIB
	XFree(visInfo);
	XMapWindow(xDisplay, win);

	m_xDisplay = xDisplay;
	m_xWindow = win;
#endif

	EGL_CHECK(eglMakeCurrent(m_display, m_surface, m_surface, m_context));

	EGLint w;
	EGLint h;
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &w);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &h);
	m_width = w;
	m_height = h;
#endif

#if LIGHTBOX_USE_SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cwarn << "SDL initialization failed: " << SDL_GetError();
		SDL_Quit();
	}

	auto info = SDL_GetVideoInfo();
	SDL_CHECK(info);

	int bpp = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	int flags = SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGLBLIT;

	m_width = c_defaultWidth;
	m_height = c_defaultHeight;
	SDL_CHECK(SDL_SetVideoMode(m_width, m_height, bpp, flags));

#endif
	glViewport(0, 0, m_width, m_height);
#if 0
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(0);

	GLuint p = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLchar const* vsc = "attribute vec3 geometry; void main() { gl_Position.xyz = geometry; }";
	glShaderSource(vs, 1, &vsc, nullptr);
	glCompileShader(vs);
	glAttachShader(p, vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar const* fsc = "precision mediump float; void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }";
	glShaderSource(fs, 1, &fsc, nullptr);
	glCompileShader(fs);
	glAttachShader(p, fs);
	glLinkProgram(p);
	glUseProgram(p);

	GLuint geom;
	glGenBuffers(1, &geom);
	glBindBuffer(GL_ARRAY_BUFFER, geom);
	float geomdata[] = {0, -1, 0, -1, 1, 0, 1, 1, 0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(geomdata), geomdata, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(p, "geometry"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

#if LIGHTBOX_USE_SDL
	SDL_GL_SwapBuffers();
#elif LIGHTBOX_USE_EGL
	eglSwapBuffers(m_display, m_surface);
#endif
#endif

	m_widthMM = m_width;
	m_heightMM = m_height;
}

Lightbox::Display::~Display()
{
#if LIGHTBOX_USE_EGL
	if (m_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT)
			eglDestroyContext(m_display, m_context);
		if (m_surface != EGL_NO_SURFACE)
			eglDestroySurface(m_display, m_surface);
		eglTerminate(m_display);
#if LIGHTBOX_USE_XLIB
		XCloseDisplay((::Display*)m_xDisplay);
#endif
	}
#endif
#if LIGHTBOX_USE_SDL
	SDL_Quit();
#endif
}

void Lightbox::Display::update()
{
#if LIGHTBOX_USE_EGL
	eglSwapBuffers(m_display, m_surface);
#endif
#if LIGHTBOX_USE_SDL
	SDL_GL_SwapBuffers();
#endif
	m_animating = false;
}

