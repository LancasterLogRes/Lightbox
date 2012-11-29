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

void ViewBody::draw(Context _c)
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

void HorizontalLayerout::layout(fSize _s)
{
	if (View v = m_view.lock())
	{
		float maxChild = 0.f;
		fSize minChildren(0, 0);
		std::vector<float> childMins;
		childMins.reserve(v->children().size());
		for (auto const& c: v->children())
		{
			auto m = c->minimumSize();
			minChildren = fSize(minChildren.w() + m.w(), max(minChildren.h(), m.h()));
			maxChild = max(maxChild, m.w());
			childMins.push_back(m.w());
		}
		float perfect = maxChild * v->children().size();
		{
			float overMinimum = lext(_s.w(), minChildren.w(), perfect);
			unsigned i = 0;
			fCoord cursor(0, 0);
			for (auto const& c: v->children())
			{
				fSize s;
				if (_s.w() < perfect)
					s = fSize(lerp(overMinimum, childMins[i], maxChild), _s.h());
				else
					s = fSize(_s.w() / v->children().size(), _s.h());
				c->setGeometry(fRect(cursor, s));
				cursor.setX(cursor.x() + s.w());
				i++;
			}
		}
	}
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
