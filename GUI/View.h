#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <boost/utility.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/any.hpp>
#include <Numeric/Rect.h>
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

struct Context
{
	fRect clip;		// in root coords
	fSize offset;	// from root topLeft.

	void rect(fRect _r) const;
	void rect(fRect _r, Color _c) const;
	void rect(fRect _r, Color _c, float _gradient) const;
	void rect(fRect _r, Program const& _p) const;
	void disc(fCoord _center, float _r) const;
	void disc(fCoord _center, fSize _r, Color _c) const;
	void disc(fCoord _center, float _r, Color _c) const;
	void disc(fCoord _center, float _r, Program const& _p) const;
	void circle(fCoord _center, fSize _r, Color _c, float _size = 1.f) const;
	void blit(Texture2D const& _tex, fCoord _pos = fCoord(0, 0)) const;
	void text(Font const& _f, fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;
};

class ViewBody;
typedef boost::intrusive_ptr<ViewBody> View;

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
	_out << toString(_v);
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

class ViewBody: public boost::noncopyable
{
	template <class _T, class _I> friend class ViewCreator;
	friend class GUIApp;
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
	void setVisible(bool _vi) { if (m_isVisible != _vi) { m_isVisible = _vi; visibilityChanged(); } }
	void setChildIndex(ChildIndex _i) { if (m_parent) { m_references++; m_parent->m_children.erase(this); m_childIndex = _i; m_parent->m_children.insert(this); m_references--; } else m_childIndex = _i; noteMetricsChanged(); }
	void setLayout(Layout* _newLayout) { m_layout = _newLayout; m_layout->m_view = this; noteLayoutDirty(); }
	void setStretch(float _stretch) { m_stretch = _stretch; noteMetricsChanged(); }
	void setPadding(float _padding) { m_padding = fVector4(_padding, _padding, _padding, _padding); noteMetricsChanged(); }

	View withLayout(Layout* _newLayout) { setLayout(_newLayout); return this; }
	View withPadding(float _padding) { setPadding(_padding); return this; }
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
			if (auto p = dynamic_cast<decltype(_T().get())>(c.get()))
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
				if (p->property<_U>(_propertyName) == _propertyValue)
					return _T(p);
			}
			else if (_T r = c->findFirst<_T, _U>(_propertyName, _propertyValue))
				return r;
		return _T();
	}

	bool isEnabled() const { return m_isEnabled; }
	bool isVisible() const { return m_isVisible; }
	ChildIndex childIndex() const { return m_childIndex; }
	fRect geometry() const { return m_geometry; }
	View parent() const { return View(m_parent); }
	Layout* layout() const { return m_layout; }
	float stretch() const { return m_stretch; }
	fVector4 padding() const { return m_padding; }

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

	virtual void draw(Context const& _c);
	virtual bool event(Event*) { return false; }
	virtual void resized() { m_visibleLayoutChanged = true; relayout(); update(); }
	virtual void visibilityChanged() { m_visibleLayoutChanged = true; update(); }

	virtual fSize specifyMinimumSize(fSize) const;	// default is determined by layout.
	virtual fSize specifyMaximumSize(fSize) const;	// default is determined by layout.
	virtual fSize specifyFit(fSize _space) const;	// default is determined by layout.

private:
	void checkCache();
	void cleanCache();
	void gatherDrawers(std::vector<ViewBody*>& _l, fCoord _o = fCoord(0, 0), bool _ancestorVisibileLayoutChanged = false);

	fRect m_geometry;					// Relative to the parent's coordinate system. (0, 0) is at parent's top left.
	ViewBody* m_parent;					// Raw pointers are only allowed here because the parent will remove itself from here in its destructor.
	unsigned m_references;
	bool m_constructionReference;
	ViewSet m_children;
	std::unordered_map<std::string, boost::any> m_misc;
	Layout* m_layout;
	ChildIndex m_childIndex;
	float m_stretch;
	fVector4 m_padding;
	bool m_isVisible;
	bool m_isEnabled;
	mutable bool m_dirty;
	mutable bool m_visibleLayoutChanged;
	bool m_isCorporal;
	fCoord m_globalPosAsOfLastGatherDrawers;
	bool m_wasDirty;
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
