#include <regex>
#include <LGL.h>
#include <App/Display.h>
#include <App/AppEngine.h>
#include <Common/StreamIO.h>
#include "GUIApp.h"
#include "Slate.h"
#include "View.h"
using namespace std;
using namespace Lightbox;

void Layer::refresh()
{
	if (GUIApp::get() && GUIApp::joint().display)
		GUIApp::joint().display->setOneOffAnimating();
}

ViewBody::ViewBody():
	m_parent(nullptr),
	m_references(0),
	m_constructionReference(false),
	m_layout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_isShown(true),
	m_isEnabled(true),
	m_isAlive(false),
	m_visibleLayoutChanged(true),
	m_graphicsInitialized(false),
	m_layers(1)
{
	// Allows it to always stay alive during the construction process, even if an intrusive_ptr
	// is destructed within it. This must be explicitly decremented at some point afterwards,
	// once we're sure there's another instrusive_ptr floating around (e.g. after construction
	// in the doCreate method).
	// NOTE: If you must subclass and use new directly outside of create, make sure you call
	// doneConstruction() (and store the View object it returns) sometime after the new call
	// or finalConstructor() at the end of the final constructor of the class.
	m_references = 1;
	m_constructionReference = true;
}

ViewBody::~ViewBody()
{
	setAlive(false);

	delete m_layout;
	m_layout = nullptr;

	clearChildren();
}

void ViewBody::executeDraw(Slate const& _c, unsigned _layer)
{
	if (m_graphicsInitialized)
		draw(_c, _layer);
}

void ViewBody::clearChildren()
{
	// Mustn't use a for loop as the iterator will become invalid as the child removes
	// itself from it in the setParent call.
	while (m_children.size())
	{
		// The child must stay valid for the duration of the call...
		View c = *m_children.begin();
		c->setParent(nullptr);
	}
}

void ViewBody::update(int _layer)
{
	if (_layer >= 0)
	{
		if (_layer < (int)m_layers.size())
			m_layers[_layer].update();
			// else graphics might not yet be initialized.
	}
	else
		for (auto& l: m_layers)
			l.update();
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

			// We can safely kill this safety measure now.
			finalConstructor();
		}
		if (op)
			op->noteLayoutDirty();
		if (m_parent)
			m_parent->noteLayoutDirty();
	}
}

void ViewBody::setLayers(Layers const& _l)
{
	m_layers = _l;
	m_visibleLayoutChanged = true;
	update();
}

bool ViewBody::gatherDrawers(std::vector<ViewLayerPtr>& _l, unsigned _layer, fCoord _o, bool _ancestorVisibleLayoutChanged)
{
	bool ret = m_visibleLayoutChanged;
	m_globalRectMM = m_geometry.translated(_o);
	m_globalRect = GUIApp::joint().display->toPixels(m_globalRectMM);
	for (unsigned i = 0; i < m_layers.size(); ++i)
		m_layers[i].m_globalLayer = m_globalRect.outset(m_layers[i].overdraw());

	// Visibility is inherited, so if we draw and have not had our visibility changed directly
	// but an ancestor has, we must inherit that invalidity.
	// This is particularly important if the ancestor doesn't draw, since it will go unnoticed
	// during the compositing stage otherwise.
	_ancestorVisibleLayoutChanged |= m_visibleLayoutChanged;

	if (draws() && m_isShown)
	{
		// If we're going to draw, we need to flag ourselves as visible-layout-changed if us or
		// any of our ancestors have had their layout visibly changed.
		m_visibleLayoutChanged = _ancestorVisibleLayoutChanged;
		_l += ViewLayerPtr((ViewBody*)this, 0u);
	}
	else
		// If it can't be reset by the compositor because it's not in the view tree, then we'll
		// need to reset it here; if it's visible, then its attribute may yet alter its children.
		// Otherwise, the tree traversal ends here.
		m_visibleLayoutChanged = false;

	if (m_isShown)
		for (auto const& ch: m_children)
			ret = ch->gatherDrawers(_l, _layer, m_globalRectMM.pos(), _ancestorVisibleLayoutChanged) || ret;
	return ret;
}

void ViewBody::draw(Slate const&, unsigned)
{
}

bool ViewBody::sensesEvent(Event* _e)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	finalConstructor();

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		return m_isShown && m_isEnabled && m_geometry.contains(e->mmLocal());
	else
		return true;
}

bool ViewBody::handleEvent(Event* _e)
{
	if (event(_e))
		return true;

	auto p = m_geometry.pos();
	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->m_mmLocal -= p;

	bool ret = false;
	for (auto const& ch: m_children)
		if (ch->sensesEvent(_e) && ch->handleEvent(_e))
		{
			ret = true;
			break;
		}

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->m_mmLocal += p;

	return ret;
}

void ViewBody::setAlive(bool _alive)
{
	if (_alive == m_isAlive)
		return;
	m_isAlive = _alive;
	if (_alive)
		GUIApp::get()->registerAlive(this);
	else
		GUIApp::get()->unregisterAlive(this);
}

void ViewBody::relayout()
{
	if (m_layout)
	{
		m_layout->layout(m_geometry.size());
		update();
	}
}

fSize ViewBody::specifyMinimumSize(fSize _s) const
{
	if (m_layout)
		return m_layout->minimumSize(_s);
	else
		return fSize(0.f, 0.f);
}

fSize ViewBody::specifyMaximumSize(fSize _s) const
{
	if (m_layout)
		return m_layout->maximumSize(_s);
	else
		return _s;
}

fSize ViewBody::specifyFit(fSize _space) const
{
	if (m_layout)
		return m_layout->fit(_space);
	else
		return _space;
}

fCoord ViewBody::globalPos() const
{
	fCoord ret = geometry().pos();
	for (View p = parent(); p; p = p->parent())
		ret += p->geometry().pos();
	return ret;
}

void ViewBody::lockPointer(int _id)
{
	GUIApp::get()->lockPointer(_id, this);
}

bool ViewBody::pointerLocked(int _id)
{
	return GUIApp::get()->pointerLocked(_id, this);
}

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, this);
}

string ViewBody::name() const
{
	string ret = demangled(typeid(*this).name());
	if (ret.substr(0, 10) == "Lightbox::")
		ret = ret.substr(10);
	if (ret.substr(ret.size() - 4) == "Body")
		ret = ret.substr(0, ret.size() - 4);
	return ret;
}

/*namespace Lightbox
{
template <class _S> _S& operator<<(_S& _out, boost::any const& _a)
{
	try	{ _out << boost::any_cast<int>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<unsigned>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<char>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<string>(_a); return _out; } catch (...) {}
	_out << "#Unknown";
	return _out;
}
}*/

std::string Lightbox::toString(View const& _v, std::string const& _insert)
{
	std::stringstream out;
	out << (_v->m_isEnabled ? "EN" : "--") << " "
		<< (_v->m_isShown ? "VIS" : "hid") << " "
		<< (_v->m_graphicsInitialized ? "GFX " : "___ ")
		<< "*" << _v->m_layers.size() << " [";
	for (auto const& i: _v->m_layers)
		out << (i.isDirty() ? i.isShown() ? "!" : "_" : i.isShown() ? "*" : "+");
	out	<< (_v->m_visibleLayoutChanged ? " LAY" : " lay") << "] "
		<< _insert
		<< _v->name() << ": "
//		<< _v->minimumSize() << " -> "
//		<< _v->maximumSize() << "  "
		<< _v->geometry() << " (" << _v->m_children.size() << " children)";
	out << (void*)_v.get();
	for (auto const& i: _v->m_misc)
	{
		try	{ int x = boost::any_cast<int>(i.second); out << " " << i.first << "=" << x; } catch (...) {}	// i.first (string) BAD!!!
		try	{ string x = boost::any_cast<string>(i.second); out << " " << i.first << "=" << x; } catch (...) {}
	}
	return out.str();
}

void Lightbox::debugOut(View const& _v, std::string const& _indent)
{
	cnote << toString(_v, _indent);
	for (auto const& c: _v->children())

		debugOut(c, _indent + "  ");
}
