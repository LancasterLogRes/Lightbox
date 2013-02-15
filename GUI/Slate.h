#pragma once
#include <boost/intrusive_ptr.hpp>
#include <Common/Color.h>
#include <Numeric/Rect.h>
#include <Numeric/Ellipse.h>
#include <LGL/Program.h>
#include "Font.h"

namespace Lightbox
{

class RenderToTextureSlate;
class ViewBody;
class GUIApp;


/** Drawing class.
 * Operates on a integer rect that's pixel-aligned.
 * Primary functions are defined in terms of pixels, though mm functions also exist.
 * For pixel functions, all exist as integer, though most are overloaded with float
 * variants, too.
 */
class Slate
{
	friend class RenderToTextureSlate;
	friend class ViewBody;
	friend class GUIApp;

public:
	// Shouldn't need to use these, but in case you're interested...
	iRect main() const { return iRect(m_active.size()); }
	iRect limits() const { return iRect(m_canvas.pos() - (iSize)m_active.pos(), m_canvas.size()); }

	iSize toPixels(fSize _mm) const;
	iCoord toPixels(fCoord _mm) const;
	iEllipse toPixels(fEllipse _mm) const { return (iEllipse)toPixelsF(_mm); }
	iRect toPixels(fRect _mm) const { return (iRect)toPixelsF(_mm); }

	fSize toPixelsF(fSize _mm) const;
	fCoord toPixelsF(fCoord _mm) const;
	fEllipse toPixelsF(fEllipse _mm) const { return fEllipse(toPixelsF(_mm.center()), toPixelsF(_mm.radii())); }
	fRect toPixelsF(fRect _mm) const { return fRect(toPixelsF(_mm.pos()), toPixelsF(_mm.size())); }

	// Pixel drawing functions.
	void rect(fRect _r) const;
	void rect(iRect _r) const { rect(fRect(_r)); }
	void rect(fRect _r, Color _c) const;
	void rect(iRect _r, Color _c) const { rect(fRect(_r), _c); }
	void rect(fRect _r, Color _c, float _gradient) const;
	void rect(iRect _r, Color _c, float _gradient) const { rect(fRect(_r), _c, _gradient); }
	void xRule(fRect _r, float _yFraction, float _h, Color _c) const { rect(_r.lerp(0, _yFraction, 1, _yFraction).outset(0, _h / 2), _c); }
	void yRule(fRect _r, float _xFraction, float _w, Color _c) const { rect(_r.lerp(_xFraction, 0, _xFraction, 1).outset(_w / 2, 0), _c); }
	void rectOutline(fRect _inner, fMargin _outset, Color _c) const;
	void rectOutline(iRect _inner, iMargin _outset, Color _c) const { rectOutline((fRect)_inner, (fMargin)_outset, _c); }
	void rectInline(fRect _outer, fMargin _inset, Color _c) const { rectOutline(_outer.inset(_inset), _inset, _c); }
	void rectInline(iRect _outer, iMargin _inset, Color _c) const { rectInline((fRect)_outer, (fMargin)_inset, _c); }
	void disc(fEllipse _r) const;
	void disc(iEllipse _r) const { disc(fEllipse(_r)); }
	void disc(fEllipse _r, Color _c) const;
	void disc(iEllipse _r, Color _c) const { disc(fEllipse(_r), _c); }
	void circle(fEllipse _r, float _size, Color _c) const;
	void circle(fEllipse _r, float _size) const;
	// Note: Below here it doesn't make sense to use float coordinates as the operation must be pixel aligned.
	void text(Font const& _f, iCoord _anchor, std::string const& _text, Color _c = Black) const;
	void blit(Texture2D const& _tex, iCoord _dest = iCoord(0, 0)) const { blit(_tex, iRect(_dest, iSize(0, 0))); }
	void blit(Texture2D const& _tex, iRect _dest) const { blit(iRect(0, 0, 0, 0), _tex, _dest); }
	void blit(iRect _src, Texture2D const& _tex, iCoord _dest) const { blit(_src, _tex, iRect(_dest, iSize(0, 0))); }
	void blit(iRect _src, Texture2D const& _tex, iRect _dest = iRect(0, 0, 0, 0)) const;
	void glowThumb(iCoord _pos, Color _c, float _overglow = 0) const;
	void glowRect(iRect _topLeftOfLightOutline, Color _col, float _overglow) const;
	void glowRectOutline(iRect _inner, Color _col, float _overglow = 1.f) const;
	void glowRectInline(iRect _outer, Color _col, float _overglow = 1.f) const;

	// MM drawing functions.
	void mmRect(fRect _r) const { rect(toPixelsF(_r)); }
	void mmRect(fRect _r, Color _c) const { rect(toPixelsF(_r), _c); }
	void mmRect(fRect _r, Color _c, float _gradient) const { rect(toPixelsF(_r), _c, _gradient); }
	void mmDisc(fEllipse _r) const { disc(toPixelsF(_r)); }
	void mmDisc(fEllipse _r, Color _c) const { disc(toPixelsF(_r), _c); }
	void mmCircle(fEllipse _r, float _size, Color _c) const { circle(toPixelsF(_r), _size, _c); }
	void mmCircle(fEllipse _r, float _size) const { circle(toPixelsF(_r), _size); }
	void mmXRule(fRect _r, float _yFraction, float _h, Color _c) const { xRule(toPixelsF(_r), _yFraction, toPixelsF(fSize(0, _h)).h(), _c); }
	void mmYRule(fRect _r, float _xFraction, float _w, Color _c) const { yRule(toPixelsF(_r), _xFraction, toPixelsF(fSize(_w, 0)).w(), _c); }
	void mmText(Font const& _f, fCoord _anchor, std::string const& _text, Color _c = Black) const { text(_f, toPixels(_anchor), _text, _c); }
	void mmBlit(Texture2D const& _tex, fCoord _pos) const { blit(_tex, toPixels(_pos)); }
	void mmGlowThumb(fCoord _pos, Color _c, float _overglow = 0) const { glowThumb(toPixels(_pos), _c, _overglow); }

private:
	Slate() {}
	Slate(iRect _active, iRect _canvas): m_active(_active), m_canvas(_canvas) {}

	fCoord mmToDevice(fCoord _mm) const { return toPixelsF(_mm) + fCoord(m_active.pos()); }

	iRect m_active;	///< Basic (non-overdrawn) view rect, in device coordinates. If there's overdraw (or it's not drawing to texture), it'll have a positive topleft.
	iRect m_canvas;	///< Full (overdrawn) view rect, in device coordinates. Zero top-left if it's drawing to texture.
};

}


