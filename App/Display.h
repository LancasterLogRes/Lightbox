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
	float widthMM() const { return m_widthMM; }
	float heightMM() const { return m_heightMM; }

	uSize sizePixels() const { return uSize(m_width, m_height); }
	fSize sizeMM() const { return fSize(m_widthMM, m_heightMM); }
	iCoord toPixels(fCoord _mm) const { return iCoord(round(_mm.x() / m_widthMM * m_width), round(_mm.y() / m_heightMM * m_height)); }
	iSize toPixels(fSize _mm) const { return iSize(ceil(_mm.w() / m_widthMM * m_width), ceil(_mm.h() / m_heightMM * m_height)); }

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
	float m_widthMM;
	float m_heightMM;

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

