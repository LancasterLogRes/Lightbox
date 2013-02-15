#pragma once
#include <boost/intrusive_ptr.hpp>
#include <Common/Color.h>
#include <Numeric/Rect.h>
#include <Numeric/Ellipse.h>
#include <LGL/Program.h>
#include "Font.h"

namespace Lightbox
{

struct Slate
{
	fSize offset;	///< from root topLeft. In MM. @deprecated Completely useless!

	iRect active;	///< Basic (non-overdrawn) view rect, in device coordinates. If there's overdraw (or it's not drawing to texture), it'll have a positive topleft.
	iRect canvas;	///< Full (overdrawn) view rect, in device coordinates. Zero top-left if it's drawing to texture.

	Slate() {}
	Slate(iRect _active, iRect _canvas): active(_active), canvas(_canvas) {}

	iSize offsetPixels() const { return iSize(canvas.left(), canvas.top()); }

	iSize pixels(fSize _mm) const { return toPixels(_mm); }
	iCoord pixels(fCoord _mm) const { return toPixels(_mm); }
	fSize pixelsF(fSize _mm) const { return toPixelsF(_mm); }
	fCoord pixelsF(fCoord _mm) const { return toPixelsF(_mm); }

	iSize toPixels(fSize _mm) const;
	iCoord toPixels(fCoord _mm) const;
	fSize toPixelsF(fSize _mm) const;
	fCoord toPixelsF(fCoord _mm) const;

	fCoord toDevice(fCoord _mm) const { return pixelsF(_mm) + fCoord(active.pos()); }

	// Pixel functions - TODO split into proxy and px float param.
	void rectInline(iRect _outer, iMargin _inset, Color _c) const { rectOutline(_outer.inset(_inset), _inset, _c); }
	void rectOutline(iRect _inner, iMargin _outset, Color _c) const;
	void text(Font const& _f, iCoord _anchor, std::string const& _text, Color _c = Black) const;

	// Pixel proxy functions.
	void rect(iRect _r) const { pxRect(fRect(_r)); }
	void rect(iRect _r, Color _c) const { pxRect(fRect(_r), _c); }
	void rect(iRect _r, Color _c, float _gradient) const { pxRect(fRect(_r), _c, _gradient); }
	void disc(iEllipse _r) const { pxDisc(fEllipse(_r)); }
	void disc(iEllipse _r, Color _c) const { pxDisc(fEllipse(_r), _c); }
	void blit(Texture2D const& _tex, iCoord _dest = iCoord(0, 0)) const { blit(_tex, iRect(_dest, iSize(0, 0))); }
	void blit(Texture2D const& _tex, iRect _dest) const { blit(iRect(0, 0, 0, 0), _tex, _dest); }
	void blit(iRect _src, Texture2D const& _tex, iCoord _dest) const { blit(_src, _tex, iRect(_dest, iSize(0, 0))); }
	void blit(iRect _src, Texture2D const& _tex, iRect _dest = iRect(0, 0, 0, 0)) const;
	void glowThumb(iCoord _pos, Color _c, float _overglow = 0) const;
	void glowRect(iRect _topLeftOfLightOutline, Color _col, float _overglow) const;
	void glowRectOutline(iRect _inner, Color _col, float _overglow = 1.f) const;
	void glowRectInline(iRect _outer, Color _col, float _overglow = 1.f) const;

	// Pixel functions that take float params.
	void pxRect(fRect _r) const;
	void pxRect(fRect _r, Color _c) const;
	void pxRect(fRect _r, Color _c, float _gradient) const;
	void pxDisc(fEllipse _r) const;
	void pxDisc(fEllipse _r, Color _c) const;

	void xRule(fRect _r, float _y, float _h, Color _c) const;
	void yRule(fRect _r, float _x, float _w, Color _c) const;

	// Deprecated - use iRect/iCoord/iEllipse versions instead.
	// Actually, no... - these are good for AA interiors; however naming convention needs to be
	// made to separate pixel-level functions (currently iRect/iCoord/iEllipse) from mm-level functions
	// (currently fRect, fCoord, fEllipse).
	void rect(fRect _r, Program const& _p = Program()) const;
	void rect(fRect _r, Color _c) const;
	void rect(fRect _r, Color _c, float _gradient) const;
	void disc(fEllipse _r, Color _c) const;
	void disc(fEllipse _r, Program const& _p) const;
	void circle(fEllipse _r, float _size, Color _c) const;
	void circle(fEllipse _r, float _size, Program const& _p) const;
	void text(Font const& _f, fCoord _anchor, std::string const& _text, Color _c = Black) const;
	void blit(Texture2D const& _tex, fCoord _pos) const;

	void glowThumb(fCoord _pos, Color _c, float _overglow = 0) const;
};

}


