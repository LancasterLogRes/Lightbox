#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Slate.h"
using namespace std;
using namespace Lightbox;

iSize Slate::toPixels(fSize _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

iCoord Slate::toPixels(fCoord _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

fSize Slate::toPixelsF(fSize _mm) const
{
	return GUIApp::joint().display->toPixelsF(_mm);
}

fCoord Slate::toPixelsF(fCoord _mm) const
{
	return GUIApp::joint().display->toPixelsF(_mm);
}

void Slate::circle(fEllipse _e, float _size, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	ProgramUser u(GUIApp::joint().flat);
	circle(_e, _size);
}

void Slate::circle(fEllipse _e, float _size) const
{
	auto vm = GUIApp::joint();
	fCoord c = _e.middle() + fCoord(m_active.pos());
	vm.offsetScale = fVector4(c.x(), c.y(), _e.rx(), _e.ry());
	LB_GL(glLineWidth, _size);
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitCircle72, 2, 0, 8);
	u.lineLoop(72);
}

void Slate::rect(fRect _r) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(fCoord(m_active.pos())).asVector4();
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Slate::rect(fRect _r, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	ProgramUser u(GUIApp::joint().flat);
	rect(_r);
}

void Slate::rect(fRect _r, Color _c, float _gradient) const
{
	GUIApp::joint().color = RGBA(_c);
	GUIApp::joint().gradient = _gradient;
	ProgramUser u(GUIApp::joint().shaded);
	rect(_r);
}

void Slate::disc(fEllipse _e) const
{
	auto vm = GUIApp::joint();
	fCoord c = _e.middle() + fCoord(m_active.pos());
	vm.offsetScale = fVector4(c.x(), c.y(), _e.rx(), _e.ry());
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Slate::disc(fEllipse _e, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	ProgramUser u(GUIApp::joint().flat);
	disc(_e);
}

void Slate::rectOutline(fRect _inner, fMargin _outset, Color _c) const
{
	auto vm = GUIApp::joint();

	// Left bar
	fRect lb(_inner.left() - _outset.left(), _inner.top() - _outset.top(), _outset.left(), _inner.height() + _outset.extraHeight());
	fRect rb(_inner.right(), _inner.top() - _outset.top(), _outset.right(), _inner.height() + _outset.extraHeight());
	fRect tb(_inner.left(), _inner.top() - _outset.top(), _inner.width(), _outset.top());
	fRect bb(_inner.left(), _inner.bottom(), _inner.width(), _outset.bottom());

	// TODO: do it properly!
	ProgramUser u(vm.flat);
	vm.offsetScale = lb.translated((fCoord)m_active.pos()).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = rb.translated((fCoord)m_active.pos()).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = tb.translated((fCoord)m_active.pos()).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = bb.translated((fCoord)m_active.pos()).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Slate::text(Font const& _f, iCoord _anchor, std::string const& _text, Color _c, AnchorType _a) const
{
	(_f.isValid() ? _f : GUIApp::style().regular).draw(_anchor + m_active.pos(), _text, _c.toRGBA(), _a);
}

void Slate::glowThumb(iCoord _pos, Color _c, float _overglow) const
{
	ProgramUser u(GUIApp::joint().colorize);
	u.uniform("u_amplitude") = 1.f;
	u.uniform("u_overglow") = _overglow;
	u.uniform("u_color") = (fVector4)_c.toRGBA();
	blit(GUIApp::joint().glowThumbTex, _pos - toPixels(GUIApp::style().thumbDiameter));
}

void Slate::glowRectOutline(iRect _inner, Color _col, float _overglow) const
{
	glowRect(_inner.outset(iMargin(GUIApp::joint().lightEdgePixels.w(), GUIApp::joint().lightEdgePixels.h(), 0, 0)), _col, _overglow);
}

void Slate::glowRectInline(iRect _outer, Color _col, float _overglow) const
{
	glowRect(_outer.inset(iMargin(0, 0, GUIApp::joint().lightEdgePixels.w(), GUIApp::joint().lightEdgePixels.h())), _col, _overglow);
}

void Slate::glowRect(iRect _upperLeftEdgeOfLight, Color _col, float _overglow) const
{
	ProgramUser u(GUIApp::joint().colorize);
	u.uniform("u_color") = (fVector4)_col.toRGBA();
	u.uniform("u_amplitude") = 3.f;
	u.uniform("u_overglow") = _overglow;
	auto glowPixels = GUIApp::joint().glowPixels;
	auto lightEdgePixels = GUIApp::joint().lightEdgePixels;
	auto glowCornerTex = GUIApp::joint().glowCornerTex;
	iSize cornerSize = glowPixels * 2 + lightEdgePixels;
	blit(iRect(iCoord(1, 1), cornerSize), glowCornerTex, iRect(_upperLeftEdgeOfLight.topLeft() - glowPixels, iSize(0, 0)));
	blit(iRect(iCoord(1, 1), cornerSize).flippedHorizontal(), glowCornerTex, iRect(_upperLeftEdgeOfLight.topRight() - glowPixels, iSize(0, 0)));
	blit(iRect(iCoord(1, 1), cornerSize).flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.bottomLeft() - glowPixels, iSize(0, 0)));
	blit(iRect(iCoord(1, 1), cornerSize).flippedHorizontal().flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.bottomRight() - glowPixels, iSize(0, 0)));
	iSize extra = _upperLeftEdgeOfLight.size() - lightEdgePixels - glowPixels * 2;
	blit(iRect(glowPixels.w() * 2 + lightEdgePixels.w() + 1, 1, 1, glowPixels.h() * 2 + lightEdgePixels.h()), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() + glowPixels.w() + lightEdgePixels.w(), _upperLeftEdgeOfLight.y() - glowPixels.w(), extra.w(), 0));
	blit(iRect(glowPixels.w() * 2 + lightEdgePixels.w() + 1, 1, 1, glowPixels.h() * 2 + lightEdgePixels.h()).flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() + glowPixels.w() + lightEdgePixels.w(), _upperLeftEdgeOfLight.bottom() - glowPixels.h(), extra.w(), 0));
	blit(iRect(1, glowPixels.h() * 2 + lightEdgePixels.h() + 1, glowPixels.w() * 2 + lightEdgePixels.w(), 1), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() - glowPixels.h(), _upperLeftEdgeOfLight.y() + glowPixels.h() + lightEdgePixels.h(), 0, extra.h()));
	blit(iRect(1, glowPixels.h() * 2 + lightEdgePixels.h() + 1, glowPixels.w() * 2 + lightEdgePixels.w(), 1).flippedHorizontal(), glowCornerTex, iRect(_upperLeftEdgeOfLight.right() - glowPixels.h(), _upperLeftEdgeOfLight.y() + glowPixels.h() + lightEdgePixels.h(), 0, extra.h()));
}

void Slate::blit(iRect _src, Texture2D const& _tex, iRect _dest) const
{
	ProgramUser u(GUIApp::joint().texture, ProgramUser::AsDefault);
	if (_src.w() == 0)
		_src.setW(_tex.size().w());
	if (_src.h() == 0)
		_src.setH(_tex.size().h());
	if (_dest.w() == 0)
		_dest.setW(abs(_src.w()));
	if (_dest.h() == 0)
		_dest.setH(abs(_src.h()));

	float tw = _tex.size().w();
	float th = _tex.size().h();
	fRect o = fRect(fCoord(_dest.pos() + m_active.pos()), (fSize)_dest.size());
	std::array<float, 4 * 4> quad =
	{{
		// top left.
		_src.left() / tw, _src.top() / th, o.left(), o.top(),
		// top right.
		_src.right() / tw, _src.top() / th, o.right(), o.top(),
		// bottom left.
		_src.left() / tw, _src.bottom() / th, o.left(), o.bottom(),
		// bottom right.
		_src.right() / tw, _src.bottom() / th, o.right(), o.bottom()
	}};
	u.uniform("u_tex") = _tex;
	u.attrib("a_texCoordPosition").setStaticData(quad.data(), 4, 0);
	u.triangleStrip(4);
}
