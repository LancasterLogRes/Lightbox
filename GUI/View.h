#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <boost/utility.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/any.hpp>
#include <Numeric/Rect.h>
#include <Common/Pimpl.h>
#include <App/Display.h>
#include <Common/MemberMap.h>

#undef _P

namespace Lightbox
{

struct Event
{
	virtual ~Event() {}
};

struct TouchEvent: public Event
{
	TouchEvent(int _id, iCoord _global): id(_id), local(_global), global(_global) {}
	int id;
	fCoord local;
	iCoord global;
};

struct TouchDownEvent: public TouchEvent { TouchDownEvent(int _id, iCoord _global): TouchEvent(_id, _global) {} };
struct TouchUpEvent: public TouchEvent { TouchUpEvent(int _id, iCoord _global): TouchEvent(_id, _global) {} };
struct TouchMoveEvent: public TouchEvent { TouchMoveEvent(int _id, iCoord _global): TouchEvent(_id, _global) {} };

struct Context
{
	fRect clip;		// in root coords
	fSize offset;	// from root topLeft.
};

class ViewBody;
typedef boost::intrusive_ptr<ViewBody> View;

struct ViewSiblingsComparator
{
	inline bool operator()(View const& _a, View const& _b) const;
};

typedef std::set<View, ViewSiblingsComparator> ViewSet;

class Layerout
{
	friend class ViewBody;
public:
	virtual void layout(fSize _s) = 0;
	virtual fSize minimumSize() = 0;

protected:
	ViewBody* m_view;	// Only allowed here as this will get deleted in the view's destructor.
};

class HorizontalLayerout: public Layerout
{
public:
	virtual void layout(fSize _s);
	virtual fSize minimumSize();
};

class VerticalLayerout: public Layerout
{
public:
	virtual void layout(fSize _s);
	virtual fSize minimumSize();
};

class ViewBody: public boost::noncopyable
{
	template <class _T, class _I> friend class ViewCreator;
	friend class GUIApp;
	inline friend void intrusive_ptr_add_ref(ViewBody* _v);
	inline friend void intrusive_ptr_release(ViewBody* _v);

public:
	typedef unsigned ChildIndex;

	template <class ... _T> static View create(_T ... _args) { return doCreate<ViewBody, _T ...>(nullptr, _args ...); }
	template <class ... _T> static View spawn(View const& _parent, _T ... _args) { return doCreate<ViewBody, _T ...>(_parent, _args ...); }

	virtual ~ViewBody();

	void setParent(View const& _p);
	void setGeometry(fRect _geometry) { m_geometry = _geometry; resized(); }
	void resize(fSize _size) { auto g = m_geometry; g.resize(_size); setGeometry(g); }
	void setEnabled(bool _en) { m_isEnabled = _en; update(); }
	void setVisible(bool _vi) { m_isVisible = _vi; update(); }
	void setChildIndex(ChildIndex _i) { if (m_parent) { m_references++; m_parent->m_children.erase(this); m_childIndex = _i; m_parent->m_children.insert(this); m_references--; } else m_childIndex = _i; noteMetricsChanged(); }
	void setLayerout(Layerout* _newLayerout) { m_layerout = _newLayerout; m_layerout->m_view = this; noteLayoutDirty(); }
	void setStretch(float _stretch) { m_stretch = _stretch; noteMetricsChanged(); }
	void setPadding(float _padding) { m_padding = fVector4(_padding, _padding, _padding, _padding); noteMetricsChanged(); }

	View withLayerout(Layerout* _newLayerout) { setLayerout(_newLayerout); return View(this); }
	View withStretch(float _stretch) { setStretch(_stretch); return View(this); }

	fCoord globalPos() const;
	ViewSet children() const { return m_children; }
	bool isEnabled() const { return m_isEnabled; }
	bool isVisible() const { return m_isVisible; }
	ChildIndex childIndex() const { return m_childIndex; }
	fRect geometry() const { return m_geometry; }
	View parent() const { return View(m_parent); }
	Layerout* layerout() const { return m_layerout; }
	float stretch() const { return m_stretch; }
	fVector4 padding() const { return m_padding; }

	template <class _T> _T property(std::string const& _name) { try { return boost::any_cast<_T>(m_misc[_name]); } catch (...) { return _T(); } }
	template <class _T> View setProperty(std::string const& _name, _T const& _t) { m_misc[_name] = _t; return View(this); }

	virtual bool sensesEvent(Event* _e);
	void handleDraw(Context const& _c);
	bool handleEvent(Event* _e);

	void update();
	void relayout();

	void noteMetricsChanged() { if (m_parent) m_parent->noteLayoutDirty(); }
	void noteLayoutDirty() { noteMetricsChanged(); relayout(); }

	fSize minimumSize() const { return specifyMinimumSize(); }

//protected:
//	virtual MemberMap propertyMap() const { return MemberMap(); }

protected:
	ViewBody();

	template <class _Body, class ... _T> static boost::intrusive_ptr<_Body> doCreate(View const& _parent, _T ... _args)
	{
		auto ret = boost::intrusive_ptr<_Body>(new _Body(_args ...), false);
		ret->setParent(_parent);
		return ret;
	}

	void lockPointer(int _id);
	void releasePointer(int _id);

	virtual void draw(Context const& _c);
	virtual bool event(Event*) { return false; }
	virtual void resized() { relayout(); update(); }

	virtual fSize specifyMinimumSize() const;	// default is determined by layerout.

//private:
	fRect m_geometry;					// Relative to the parent's coordinate system. (0, 0) is at parent's top left.
	ViewBody* m_parent;					// Raw pointers are only allowed here because the parent will remove itself from here in its destructor.
	unsigned m_references;
	ViewSet m_children;
	std::unordered_map<std::string, boost::any> m_misc;
	Layerout* m_layerout;
	ChildIndex m_childIndex;
	float m_stretch;
	fVector4 m_padding;
	bool m_isVisible;
	bool m_isEnabled;
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
	template <class ... _P> ViewCreator(_P ... _args): _Inherits(_args ... ) {}

	template <class ... _T> static boost::intrusive_ptr<_ViewBody> create(_T ... _args) { return ViewBody::doCreate<_ViewBody, _T ...>(nullptr, _args ...); }
	template <class ... _T> static boost::intrusive_ptr<_ViewBody> spawn(View const& _parent, _T ... _args) { return ViewBody::doCreate<_ViewBody, _T ...>(_parent, _args ...); }
};

bool ViewSiblingsComparator::operator()(View const& _a, View const& _b) const { return _a->childIndex() < _b->childIndex(); }

inline View operator|(View const& _a, View const& _b)
{
	if (!!_a->children().size() == !!_b->children().size())
	{
		View ret = ViewBody::create();
		_a->setParent(ret);
		_b->setParent(ret);
		return ret;
	}
	else if (_a->children().size())
	{
		_b->setParent(_a);
		return _a;
	}
	else
	{
		_a->setParent(_b);
		return _b;
	}
}

inline View const& operator+=(View const& _parent, View const& _b)
{
	_b->setParent(_parent);
	return _parent;
}

}
