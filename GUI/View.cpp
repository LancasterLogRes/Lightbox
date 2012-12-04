#include <LGL.h>
#include <App/Display.h>
#include "GUIApp.h"
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

ViewBody::ViewBody():
	m_parent(nullptr),
	m_references(1),		// 1 allows it to alwys stay alive during the construction process, even if an intrusive_ptr is constructed with it. It decremented during the doCreate() method from which the constructor is always called.
	m_layerout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_padding(4.f, 4.f, 4.f, 4.f),
	m_isVisible(true),
	m_isEnabled(true)
{
}

ViewBody::~ViewBody()
{
	delete m_layerout;
	m_layerout = nullptr;
	for (auto const& c: m_children)
		c->setParent(nullptr);
}

void ViewBody::update()
{
	GUIApp::joint().display->setOneOffAnimating();
}

void ViewBody::setParent(View const& _p)
{
	if (m_parent != _p)
	{
		auto op = m_parent;
		if (m_parent)
			m_parent->m_children.erase(this);
		m_parent = _p.get();
		if (_p)
		{
			if (_p->m_children.size())
				m_childIndex = (*prev(_p->m_children.end()))->m_childIndex + 1;
			else
				m_childIndex = 0;
			_p->m_children.insert(this);
		}
		if (op)
			op->noteLayoutDirty();
		if (m_parent)
			m_parent->noteLayoutDirty();
	}
}

void ViewBody::handleDraw(Context const& _c)
{
	if (m_isVisible)
	{
		draw(_c);

		Context c = _c;
		c.offset += fSize(m_geometry.topLeft());
		for (auto const& ch: m_children)
			ch->handleDraw(c);

//		drawOverlay(_c);
		if (!m_isEnabled)
		{
			auto vm = GUIApp::joint();

			vm.offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
			vm.color = RGBA(0, 0.5f);

			ProgramUser u(vm.program);
			vm.geometry.setData(vm.unitQuad, 2);
			u.triangleStrip(4);
		}
	}
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
	if (m_isVisible && m_isEnabled)
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

fSize ViewBody::specifyMinimumSize() const
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
	GUIApp::get()->lockPointer(_id, this);
}

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, this);
}

template <class _T> double lext(_T _v, _T _lower, _T _upper)
{
	return double(_v - _lower) / (_upper - _lower);
}

fSize HorizontalLayerout::minimumSize()
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			auto p = c->padding();
			ret = fSize(ret.w() + m.w() + p.x() + p.z(), max(ret.h(), m.h() + p.y() + p.w()));
		}
	return ret;
}

fSize VerticalLayerout::minimumSize()
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			auto p = c->padding();
			ret = fSize(max(ret.w(), m.w() + p.x() + p.z()), ret.h() + m.h() + p.y() + p.w());
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
	if (m_view)
	{
		vector<float> stretch;
		stretch.reserve(m_view->children().size());
		vector<float> minima;
		minima.reserve(m_view->children().size());
		for (auto const& c: m_view->children())
			stretch += c->stretch(),
			minima += c->minimumSize().height() + c->padding().y() + c->padding().w();
		vector<float> sizes = doLayout(_s.height(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() - p.x() - p.z(), _s.h() * *i - p.y() - p.w());
			c->setGeometry(fRect(cursor, s).translated(fCoord(p.x(), p.y())));
			cursor.setY(cursor.y() + s.h() + p.y() + p.w());
			++i;
		}
	}
}

void HorizontalLayerout::layout(fSize _s)
{
	if (m_view)
	{
		vector<float> stretch;
		stretch.reserve(m_view->children().size());
		vector<float> minima;
		minima.reserve(m_view->children().size());
		for (auto const& c: m_view->children())
			stretch += c->stretch(),
			minima += c->minimumSize().width() + c->padding().x() + c->padding().z();
		vector<float> sizes = doLayout(_s.width(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() * *i - p.x() - p.z(), _s.h() - p.y() - p.w());
			c->setGeometry(fRect(cursor, s).translated(fCoord(p.x(), p.y())));
			cursor.setX(cursor.x() + s.w() + p.x() + p.z());
			++i;
		}
	}
}
