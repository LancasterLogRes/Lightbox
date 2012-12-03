#include <LGL.h>
#include <App/Display.h>
#include "GUIApp.h"
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

ViewBody::~ViewBody()
{
}

void ViewBody::update()
{
	GUIApp::joint().display->setOneOffAnimating();
}

void ViewBody::setParent(View const& _p)
{
	auto us = m_this.lock();
	assert(us);
	View p = m_parent.lock();
	if (p != _p)
	{
		if (p)
			p->m_children.erase(us);
		m_parent = _p;
		if (_p)
		{
			if (_p->m_children.size())
				m_childIndex = (*prev(_p->m_children.end()))->m_childIndex + 1;
			else
				m_childIndex = 0;
			_p->m_children.insert(us);
		}
	}
}

void ViewBody::handleDraw(Context const& _c)
{
	draw(_c);

	Context c = _c;
	c.offset += fSize(m_geometry.topLeft());
	for (auto const& ch: m_children)
		ch->handleDraw(c);

//	drawOverlay(_c);
}

void ViewBody::draw(Context const& _c)
{
	auto vm = GUIApp::joint();

	vm.offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
	vm.color = RGBA(GUIApp::style().back * .25f);

	{
		ProgramUser u(vm.program);
		vm.geometry.setData(vm.unitQuad, 2);
		u.triangleStrip(4);
	}
}

bool ViewBody::sensesEvent(Event* _e)
{
	if (auto e = dynamic_cast<TouchEvent*>(_e))
		return m_geometry.contains(e->local);
	return false;
}

bool ViewBody::handleEvent(Event* _e)
{
	if (event(_e))
		return true;

	auto p = m_geometry.pos();
	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->local -= p;

	for (auto const& ch: m_children)
		if (ch->sensesEvent(_e) && ch->handleEvent(_e))
			return true;

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->local += p;

	return false;
}

void ViewBody::relayout()
{
	if (m_layerout)
	{
		m_layerout->layout(m_geometry.size());
		update();
	}
}

fSize ViewBody::minimumSize() const
{
	if (m_layerout)
		return m_layerout->minimumSize();
	else
		return fSize(0, 0);
}

fCoord ViewBody::globalPos() const
{
	fCoord ret = m_geometry.pos();
	for (View p = parent(); p; p = p->parent())
		ret += p->globalPos();
	return ret;
}

void ViewBody::lockPointer(int _id)
{
	GUIApp::get()->lockPointer(_id, view());
}

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, view());
}

template <class _T> double lext(_T _v, _T _lower, _T _upper)
{
	return double(_v - _lower) / (_upper - _lower);
}

fSize HorizontalLayerout::minimumSize()
{
	fSize ret(0, 0);
	if (View v = m_view.lock())
		for (auto const& c: v->children())
		{
			auto m = c->minimumSize();
			ret = fSize(ret.w() + m.w(), max(ret.h(), m.h()));
		}
	return ret;
}

fSize VerticalLayerout::minimumSize()
{
	fSize ret(0, 0);
	if (View v = m_view.lock())
		for (auto const& c: v->children())
		{
			auto m = c->minimumSize();
			ret = fSize(max(ret.w(), m.w()), ret.h() + m.h());
		}
	return ret;
}

template <class _T> _T defaultTo(_T _val, _T _default, _T _invalid = (_T)0)
{
	return _val == _invalid ? _default : _val;
}

std::vector<float> doLayout(float _total, std::vector<float> const& _stretch, std::vector<float> const& _size)
{
	std::vector<float> ret(_stretch.size(), -1.f);
	float totalStretch = defaultTo(sumOf(_stretch), 1.f, 0.f);
	float totalUnreserved = 1.f;
	RESTART_OUTER:
	for (unsigned i = 0; i < ret.size(); ++i)
		if (ret[i] == -1.f && _total * _stretch[i] / totalStretch * totalUnreserved < _size[i])
		{
			totalStretch -= _stretch[i];
			ret[i] = _size[i] / _total;
			totalUnreserved -= ret[i];
			// reset back to start of the loop since totalUnreserved has now changed -
			// other children may need to reserve stretch space to honour their minimum.
			goto RESTART_OUTER;
		}

	// normalStretch now has entries (0, 1] for each child such that:
	//   minimum > stretch / totalStretch * totalUnreserved * totalPixels
	// in that case, normalStretch = the normalized stretch factor for the
	// child (i.e. minimum / totalPixels).
	for (unsigned i = 0; i < ret.size(); ++i)
		if (ret[i] == -1)
			ret[i] = _stretch[i] / totalStretch * totalUnreserved;

	return ret;
}

void VerticalLayerout::layout(fSize _s)
{
	if (View v = m_view.lock())
	{
		vector<float> stretch;
		stretch.reserve(v->children().size());
		vector<float> minima;
		minima.reserve(v->children().size());
		for (auto const& c: v->children())
			stretch += c->stretch(),
			minima += c->minimumSize().height();
		vector<float> sizes = doLayout(_s.height(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: v->children())
		{
			fSize s(_s.w(), _s.h() * *i);
			c->setGeometry(fRect(cursor, s));
			cursor.setY(cursor.y() + s.h());
			++i;
		}
	}
}

void HorizontalLayerout::layout(fSize _s)
{
	if (View v = m_view.lock())
	{
		vector<float> stretch;
		stretch.reserve(v->children().size());
		vector<float> minima;
		minima.reserve(v->children().size());
		for (auto const& c: v->children())
			stretch += c->stretch(),
			minima += c->minimumSize().width();
		vector<float> sizes = doLayout(_s.width(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: v->children())
		{
			fSize s(_s.w() * *i, _s.h());
			c->setGeometry(fRect(cursor, s));
			cursor.setX(cursor.x() + s.w());
			++i;
		}
	}
}
