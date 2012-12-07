#pragma once

#include <Numeric/Coord.h>

namespace Lightbox
{

class Display
{
public:
	Display();
	~Display();

	bool isAnimating() const { return m_animators || m_animating; }
	void setOneOffAnimating() { m_animating = true; }
	void repaint() { m_animating = true; }
	void setAnimating() { ++m_animators; }
	void releaseAnimating() { --m_animators; }

	void update();

	unsigned width() const { return m_width; }
	unsigned height() const { return m_height; }
	uSize size() const { return uSize(m_width, m_height); }

#if LIGHTBOX_USE_XLIB
	void* xDisplay() const { return m_xDisplay; }
	unsigned long xWindow() const { return m_xWindow; }
#endif

private:
	Display(Display const&) = delete;
	Display& operator=(Display const&) = delete;

	int m_animators;
	bool m_animating;

	unsigned m_width;
	unsigned m_height;

#if LIGHTBOX_USE_EGL
	/*EGLDisplay*/ void* m_display;
	/*EGLSurface*/ void* m_surface;
	/*EGLContext*/ void* m_context;
#endif

#if LIGHTBOX_USE_XLIB
	void* m_xDisplay;
	unsigned long m_xWindow;
#endif
};

}

