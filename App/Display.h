#pragma once

#include <Numeric/Coord.h>
#include <Numeric/Rect.h>

namespace nwtpi {
class OEGLWindow;
class OEGLCapabilities;
}

namespace lb
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

	float aspect() const { return m_widthMM / m_heightMM; }

	unsigned width() const { return m_width; }
	unsigned height() const { return m_height; }
	float widthMM() const { return m_widthMM; }
	float heightMM() const { return m_heightMM; }

	iSize sizePixels() const { return iSize(m_width, m_height); }
	fSize sizeMM() const { return fSize(m_widthMM, m_heightMM); }

	float toPixels(float _mm) const { assert(this); return _mm / m_widthMM * m_width; }

	iCoord toPixels(fCoord _mm) const { assert(this); return iCoord(round(_mm.x() / m_widthMM * m_width), round(_mm.y() / m_heightMM * m_height)); }
	iSize toUnalignedPixels(fSize _mm) const { assert(this); return iSize(round(_mm.w() / m_widthMM * m_width), round(_mm.h() / m_heightMM * m_height)); }
	iMargin toUnalignedPixels(fMargin _mm) const { assert(this); return iMargin(_mm.l() / m_widthMM * m_width, _mm.t() / m_heightMM * m_height, _mm.r() / m_widthMM * m_width, _mm.b() / m_heightMM * m_height); }
	iRect toPixels(fRect _mm) const { assert(this); return iRect(toPixels(_mm.topLeft()), toPixels(_mm.bottomRight())); }

	fCoord toPixelsF(fCoord _mm) const { assert(this); return fCoord(_mm.x() / m_widthMM * m_width, _mm.y() / m_heightMM * m_height); }
	fSize toPixelsF(fSize _mm) const { assert(this); return fSize(_mm.w() / m_widthMM * m_width, _mm.h() / m_heightMM * m_height); }
	fMargin toPixelsF(fMargin _mm) const { assert(this); return fMargin(_mm.l() / m_widthMM * m_width, _mm.t() / m_heightMM * m_height, _mm.r() / m_widthMM * m_width, _mm.b() / m_heightMM * m_height); }
	fRect toPixelsF(fRect _mm) const { assert(this); return fRect(toPixelsF(_mm.pos()), toPixelsF(_mm.size())); }

	fCoord fromPixels(iCoord _px) const { assert(this); return fCoord(_px.x() * m_widthMM / m_width, _px.y() * m_heightMM / m_height); }
	fSize fromPixels(iSize _px) const { assert(this); return fSize(_px.w() * m_widthMM / m_width, _px.h() * m_heightMM / m_height); }
	fMargin fromPixels(iMargin _mm) const { assert(this); return fMargin(_mm.l() * m_widthMM / m_width, _mm.t() * m_heightMM / m_height, _mm.r() * m_widthMM / m_width, _mm.b() * m_heightMM / m_height); }
	fRect fromPixels(iRect _px) const { assert(this); return fRect(fromPixels(_px.pos()), fromPixels(_px.size())); }

	float fromPixels(float _px) const { assert(this); return _px * m_widthMM / m_width; }
	fCoord fromPixels(fCoord _px) const { assert(this); return fCoord(_px.x() * m_widthMM / m_width, _px.y() * m_heightMM / m_height); }
	fSize fromPixels(fSize _px) const { assert(this); return fSize(_px.w() * m_widthMM / m_width, _px.h() * m_heightMM / m_height); }
	fMargin fromPixelsF(fMargin _mm) const { assert(this); return fMargin(_mm.l() * m_widthMM / m_width, _mm.t() * m_heightMM / m_height, _mm.r() * m_widthMM / m_width, _mm.b() * m_heightMM / m_height); }
	fRect fromPixels(fRect _px) const { assert(this); return fRect(fromPixels(_px.pos()), fromPixels(_px.size())); }

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

#if LIGHTBOX_USE_NWTPI
	nwtpi::OEGLWindow* m_oeglWindow;
	nwtpi::OEGLCapabilities* m_oeglCaps;
#endif

#if LIGHTBOX_USE_XLIB
	void* m_xDisplay;
	unsigned long m_xWindow;
#endif
};

}

