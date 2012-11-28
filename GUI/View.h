#pragma once

#include <memory>
#include <set>
#include <boost/utility.hpp>
#include <Numeric/Rect.h>
#include <Common/Pimpl.h>
#include <App/Display.h>
#include <Common/MemberMap.h>

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
typedef std::shared_ptr<ViewBody> View;

class ViewBody: public boost::noncopyable
{
public:
	template <class ... _T> static View create(View const& _parent, _T ... _args) { return doCreate<ViewBody, _T ...>(_parent, _args ...); }

	virtual ~ViewBody();

	void setParent(View const& _p);
	void setGeometry(fRect _geometry) { m_geometry = _geometry; resized(); }
	void update();

	virtual void draw(Context _c);
	virtual bool event(Event*) { return false; }
	virtual void resized() {}

	View view() const { return m_this.lock(); }
	View parent() const { return m_parent.lock(); }

	fCoord globalPos() const;

	virtual bool sensesEvent(Event* _e);
	void handleDraw(Context const& _c);
	bool handleEvent(Event* _e);

//protected:
//	virtual MemberMap propertyMap() const { return MemberMap(); }

protected:
	ViewBody() {}

	template <class _Body, class ... _T> static std::shared_ptr<_Body> doCreate(View const& _parent, _T ... _args) { auto ret = std::shared_ptr<_Body>(new _Body(_args ...)); ret->m_this = ret; ret->setParent(_parent); return ret; }

	void lockPointer(int _id);
	void releasePointer(int _id);

//private:
	fRect m_geometry;	// parent coordinate system
	std::weak_ptr<ViewBody> m_this;		// weak_ptr to itself in order to gain access to its shared_ptr.
	std::weak_ptr<ViewBody> m_parent;
	std::set<View> m_children;
};

}
