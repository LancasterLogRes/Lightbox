#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <boost/utility.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/any.hpp>
#include <Numeric/Rect.h>
#include <Numeric/Ellipse.h>
#include <Common/Pimpl.h>
#include <Common/Color.h>
#include <LGL/Global.h>
#include <LGL/Texture2D.h>
#include <App/Display.h>
#include <Common/MemberMap.h>
#include "Global.h"
#include "Event.h"
#include "Layout.h"

namespace Lightbox
{

class ViewBody;
typedef boost::intrusive_ptr<ViewBody> View;

struct Context
{
	fSize offset;	///< from root topLeft. In MM. @deprecated Completely useless!

	iRect active;	///< Basic (non-overdrawn) view rect, in device coordinates. If there's overdraw (or it's not drawing to texture), it'll have a positive topleft.
	iRect canvas;	///< Full (overdrawn) view rect, in device coordinates. Zero top-left if it's drawing to texture.

	Context() {}
	Context(View const& _v);
	Context(iRect _active, iRect _canvas): active(_active), canvas(_canvas) {}

	iSize offsetPixels() const { return iSize(canvas.left(), canvas.top()); }

	iRect pixels(ViewBody* _v) const;
	iSize pixels(fSize _mm) const;
	iCoord pixels(fCoord _mm) const;

	void rect(iRect _r, Color _c) const;
	void rect(iRect _r, Color _c, float _gradient) const;
	void text(Font const& _f, iCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;

	// Deprecated - use iRect/iCoord/iEllipse versions instead
	void rect(fRect _r) const;
	void rect(fRect _r, Color _c) const;
	void rect(fRect _r, Program const& _p) const;
	void rect(fRect _r, Color _c, float _gradient) const;
	void disc(fCoord _center, float _r) const;
	void disc(fCoord _center, fSize _r, Color _c) const;
	void disc(fEllipse _r, Color _c) const { disc(_r.pos(), _r.radii(), _c); }
	void disc(fCoord _center, float _r, Color _c) const;
	void disc(fCoord _center, float _r, Program const& _p) const;
	void circle(fCoord _center, fSize _r, Color _c, float _size = 1.f) const;
	void circle(fEllipse _p, Color _c, float _size = 1.f) const { circle(_p.pos(), _p.radii(), _c, _size); }
	void blit(Texture2D const& _tex, fCoord _pos = fCoord(0, 0)) const;
	void text(Font const& _f, fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;
};

struct ViewSiblingsComparator
{
	inline bool operator()(View const& _a, View const& _b) const;
};

typedef std::set<View, ViewSiblingsComparator> ViewSet;

class Views: public std::vector<View>
{
public:
	Views& operator,(View const& _b) { push_back(_b); return *this; }
};

inline Views operator,(View const& _a, View const& _b) { Views r; r.push_back(_a); r.push_back(_b); return r; }

void debugOut(View const& _v, std::string const& _indent = "");
std::string toString(View const& _v, std::string const& _insert = std::string());

template <class _T> _T& operator<<(_T& _out, View const& _v)
{
	if (_v)
		_out << toString(_v);
	else
		_out << "View(0)";
	return _out;
}

/* Inheriting a new type of View:
 * - Always inherit from ViewCreator<SuperView, ThisView>
 * - No getter method for event handlers.
 * - Event handler setter method takes EventHandler const& (not templated).
 * - Any setter may have corresponding with* method that have same prototype but return this.
 * - Order of methods: getters, setters, with*, others.
 * - All virtuals are protected.
 * - All fields are private.
 * - Will often have additional virtual "stateChanged" method(s):
 *   - Should generally have a bool _userEvent parameter.
 *   - Should generally call update() at end.
 *   - Should only call event handler fields  when _userEvent is true.
 *   - Should generally be called by public method that invokes action/changes state:
 *   - Said public method, if a state-setter should check for equivalence between new/old state.
 *   - Said public method should proxy a default-false _userEvent parameter.
 *   - Said public method should be called from hardware event handler with true for _userEvent.
 */

static unsigned const c_viewLayerCount = 1;

class ViewBody: public boost::noncopyable
{
	template <class _T, class _I> friend class ViewCreator;
	friend class GUIApp;
	friend class Context;
	inline friend void intrusive_ptr_add_ref(ViewBody* _v);
	inline friend void intrusive_ptr_release(ViewBody* _v);
	friend void debugOut(View const& _v, std::string const& _indent);
	friend std::string toString(View const& _v, std::string const& _insert);

public:
	typedef unsigned ChildIndex;

	template <class ... _T> static View create(_T ... _args) { return doCreate<ViewBody, _T ...>(nullptr, _args ...); }
	template <class ... _T> static View spawn(View const& _parent, _T ... _args) { return doCreate<ViewBody, _T ...>(_parent, _args ...); }

	void finalConstructor() { if (m_constructionReference) { m_references--; m_constructionReference = false; } }
	View doneConstruction() { View ret = this; finalConstructor(); return ret; }
	virtual ~ViewBody();

	std::string name() const;

	void setParent(View const& _p);
	void setGeometry(fRect _geometry) { m_geometry = _geometry; resized(); }
	void resize(fSize _size) { auto g = m_geometry; g.resize(_size); setGeometry(g); }
	void setEnabled(bool _en) { m_isEnabled = _en; update(); }
	void show(bool _vi) { if (m_isShown != _vi) { m_isShown = _vi; shownChanged(); if (!m_parent || m_parent->isVisible()) visibilityChangedRecursive(m_isShown); } }
	void hide() { show(false); }

	void setChildIndex(ChildIndex _i) { if (m_parent) { m_references++; m_parent->m_children.erase(this); m_childIndex = _i; m_parent->m_children.insert(this); m_references--; } else m_childIndex = _i; noteMetricsChanged(); }
	void setLayout(Layout* _newLayout) { m_layout = _newLayout; m_layout->m_view = this; noteLayoutDirty(); }
	void setStretch(float _stretch) { m_stretch = _stretch; noteMetricsChanged(); }

	View withChildIndex(ChildIndex _i) { setChildIndex(_i); return this; }
	View withGeometry(fRect _geometry) { setGeometry(_geometry); return this; }
	View withLayout(Layout* _newLayout) { setLayout(_newLayout); return this; }
	View withStretch(float _stretch) { setStretch(_stretch); return this; }

	fCoord globalPos() const;
	View child(unsigned _i) const { for (View v: m_children) if (_i) _i--; else return v; return View(); }
	ViewSet children() const { return m_children; }
	template <class _T> std::vector<_T> find()
	{
		std::vector<_T> ret;
		for (auto const& c: m_children)
			if (auto p = dynamic_cast<decltype(&*_T())>(c.get()))
				ret += _T(p);
			else
				ret += c->find<_T>();
		return ret;
	}

	template <class _T> _T findFirst()
	{
		for (auto const& c: m_children)
			if (auto p = dynamic_cast<decltype(&*_T())>(c.get()))
				return _T(p);
			else if (_T r = c->findFirst<_T>())
				return r;
		return _T();
	}

	template <class _T, class _U> _T findFirst(std::string const& _propertyName, _U const& _propertyValue)
	{
		for (auto const& c: m_children)
			if (auto p = dynamic_cast<decltype(_T().get())>(c.get()))
			{
				if (p->hasProperty<_U>(_propertyName) && p->property<_U>(_propertyName) == _propertyValue)
					return _T(p);
			}
			else if (_T r = c->findFirst<_T, _U>(_propertyName, _propertyValue))
				return r;
		return _T();
	}

	void clearChildren();

	bool isEnabled() const { return m_isEnabled; }
	bool isShown() const { return m_isShown; }
	bool isHidden() const { return !m_isShown; }
	bool isVisible() const { return m_isShown && (!m_parent || parent()->isVisible()); }
	ChildIndex childIndex() const { return m_childIndex; }
	fRect geometry() const { return m_geometry; }
	View parent() const { return View(m_parent); }
	Layout* layout() const { return m_layout; }
	float stretch() const { return m_stretch; }
	iRect rect() const { return iRect(iCoord(0, 0), m_globalRect.size()); }
	iRect canvas() const { return rect().outset(m_overdraw); }

	template <class _T> _T property(std::string const& _name) { try { return boost::any_cast<_T>(m_misc[_name]); } catch (...) { return _T(); } }
	template <class _T> bool hasProperty(std::string const& _name) { if (m_misc.count(_name)) try { boost::any_cast<_T>(m_misc[_name]); return true; } catch (...) {} return false; }
	template <class _T> View setProperty(std::string const& _name, _T const& _t) { m_misc[_name] = _t; return View(this); }

	virtual bool sensesEvent(Event* _e);
	void handleDraw(Context const& _c);
	bool handleEvent(Event* _e);

	void update();
	void relayout();

	void noteMetricsChanged() { if (m_parent) m_parent->noteLayoutDirty(); }
	void noteLayoutDirty() { noteMetricsChanged(); relayout(); }

	fSize minimumSize(fSize _space = fSize(32768.f, 32768.f)) const { return specifyMinimumSize(_space); }
	fSize maximumSize(fSize _space = fSize(32768.f, 32768.f)) const { return specifyMaximumSize(_space); }
	fSize fit(fSize _space) const { return specifyFit(_space); }

//protected:
//	virtual MemberMap propertyMap() const { return MemberMap(); }

protected:
	ViewBody();

	template <class _Body, class ... _T> static boost::intrusive_ptr<_Body> doCreate(View const& _parent, _T ... _args)
	{
		auto ret = boost::intrusive_ptr<_Body>(new _Body(_args ...));
		ret->finalConstructor();
		ret->setParent(_parent);
		return ret;
	}

	void setCorporal(bool _enableDraw) { m_isCorporal = _enableDraw; }
	void lockPointer(int _id);
	void releasePointer(int _id);
	bool pointerLocked(int _id);

	virtual iMargin prepareDraw(int _layer) { (void)_layer; return m_overdraw; }
	virtual void draw(Context const& _c, int _layer) { (void)_layer; draw(_c); }
	virtual void draw(Context const& _c);
	virtual bool event(Event*) { return false; }
	virtual void resized() { m_visibleLayoutChanged = true; relayout(); update(); }
	virtual void shownChanged() { m_visibleLayoutChanged = true; update(); }
	virtual void visibilityChanged() {}

	virtual fSize specifyMinimumSize(fSize) const;	// default is determined by layout.
	virtual fSize specifyMaximumSize(fSize) const;	// default is determined by layout.
	virtual fSize specifyFit(fSize _space) const;	// default is determined by layout.

	virtual void initGraphics() {}
	virtual void finiGraphics() {}

private:
	void checkCache();
	void cleanCache();
	bool gatherDrawers(std::vector<ViewBody*>& _l, fCoord _o = fCoord(0, 0), bool _ancestorVisibileLayoutChanged = false);

	void executeDraw(Context const& _c, int _layer);
	void initGraphicsRecursive() { if (!m_graphicsInitialized) { initGraphics(); m_graphicsInitialized = true; } for (auto const& c: m_children) c->initGraphicsRecursive(); }
	void finiGraphicsRecursive() { for (auto const& c: m_children) c->finiGraphicsRecursive(); if (m_graphicsInitialized) { finiGraphics(); m_graphicsInitialized = false; } }
	void visibilityChangedRecursive(bool _newRootVisibility) { for (auto const& c: m_children) if (c->m_isShown) c->visibilityChangedRecursive(_newRootVisibility); visibilityChanged(); }

	fRect m_geometry;					///< Relative to the parent's coordinate system, in mm. (0, 0) is at parent's top left. Typically unused.
	ViewBody* m_parent;					///< Raw pointers are only allowed here because the parent will remove itself from here in its destructor.
	unsigned m_references;				///< Number of references held to this object.
	bool m_constructionReference;		///< True if a reference in still held in order to guarantee the object isn't destructed over the course of its construction.
	ViewSet m_children;					///< The set of children (ordered by m_childIndex).
	std::unordered_map<std::string, boost::any> m_misc;	///< Miscellaneous properties.
	Layout* m_layout;					///< The layout object used to layout the children of this View. Allowed to be nullptr, in which case the children are left alone.
	ChildIndex m_childIndex;			///< Our index among our siblings. Used for layout ordering. Not necessarily contiguous between ordered siblings.
	float m_stretch;					///< Stretch factor - used in layout algorithms.
	bool m_isShown;						///< Whether we would indeed draw ourselves should our parent be visible. Necessary but not sufficient for being visible.
	bool m_isEnabled;					///< Whether we are interactive. Doesn't affect visibility.
	mutable bool m_dirty;				///< True if a redraw would result in a different canvas to the last.
	mutable bool m_visibleLayoutChanged;	///< True if our, or any of our ancestors', layout has been changed and said view is visible.
	bool m_isCorporal;					///< True if our draw call does anything.
	bool m_wasDirty;					///< True if we were dirty but have since been redrawn but not yet recached.
	bool m_graphicsInitialized;			///< True if we have initialized graphics (with initGraphics()) and not subsequently finalized (with finiGraphics()).

	iMargin m_overdraw;					///< The margin of overdraw that we wanted as of the last call to prepareDraw(), stored in pixels.

	fRect m_globalRectMM;				///< Our basic footprint in device coordinates in mm; this doesn't include overdraw. Up to date at time of last call to gatherDrawers().
	iRect m_globalRect;					///< Our basic footprint in device coordinates; this doesn't include overdraw. Up to date at time of last call to gatherDrawers().
	iRect m_globalCanvas;				///< Our full footprint in device coordinates; includes overdraw. Up to date at time of last call to gatherDrawers().
};

inline void intrusive_ptr_add_ref(ViewBody* _v)
{
	++_v->m_references;
}

inline void intrusive_ptr_release(ViewBody* _v)
{
	if (!--_v->m_references)
		delete _v;
}

template <class _Inherits, class _ViewBody>
class ViewCreator: public _Inherits
{
public:
	typedef std::function<void(_ViewBody*)> EventHandler;
	typedef _ViewBody This;
	typedef ViewCreator<_Inherits, _ViewBody> Super;

	template <class ... _P> ViewCreator(_P ... _args): _Inherits(_args ... ) {}

	template <class ... _T> static boost::intrusive_ptr<_ViewBody> create(_T ... _args) { return ViewBody::doCreate<_ViewBody, _T ...>(nullptr, _args ...); }
	template <class ... _T> static boost::intrusive_ptr<_ViewBody> spawn(View const& _parent, _T ... _args) { return ViewBody::doCreate<_ViewBody, _T ...>(_parent, _args ...); }
};

bool ViewSiblingsComparator::operator()(View const& _a, View const& _b) const { return _a->childIndex() < _b->childIndex(); }

inline View const& operator+=(View const& _parent, View const& _b)
{
	_b->setParent(_parent);
	return _parent;
}

inline View const& operator+=(View const& _parent, Views const& _b)
{
	for (auto const& c: _b)
		c->setParent(_parent);
	return _parent;
}

}
