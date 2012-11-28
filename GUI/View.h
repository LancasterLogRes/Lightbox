#pragma once

#include <memory>
#include <set>
#include <boost/utility.hpp>
#include <Numeric/Rect.h>
#include <Common/Pimpl.h>
#include <LGL.h>
#include <App/Display.h>
#include <Common/MemberMap.h>

namespace Lightbox
{

class ViewManager
{
public:
	static void init(Display& _d) { if (!s_this) s_this = new ViewManager(_d); }
	static ViewManager* get() { return s_this; }

	Display* display;
	uSize displaySize;
	UniformPage uniforms;
	PagedUniform offsetScale;
	PagedUniform color;
	Buffer<float> geometryBuffer;
	Program program;
	Attrib geometry;
	Font defaultFont;

private:
	ViewManager(Display& _d);
	static ViewManager* s_this;
};

struct Event
{
	virtual ~Event() {}
};

struct TouchEvent: public Event
{
	TouchEvent(int _id, iCoord _local): id(_id), local(_local) {}
	int id;
	iCoord local;
};

struct TouchDownEvent: public TouchEvent { TouchDownEvent(int _id, iCoord _local): TouchEvent(_id, _local) {} };
struct TouchUpEvent: public TouchEvent { TouchUpEvent(int _id, iCoord _local): TouchEvent(_id, _local) {} };
struct TouchMoveEvent: public TouchEvent { TouchMoveEvent(int _id, iCoord _local): TouchEvent(_id, _local) {} };

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

	void handleDraw(Context const& _c);
	bool handleEvent(Event* _e);

//protected:
//	virtual MemberMap propertyMap() const { return MemberMap(); }

protected:
	ViewBody() {}

	template <class _Body, class ... _T> static std::shared_ptr<_Body> doCreate(View const& _parent, _T ... _args) { auto ret = std::shared_ptr<_Body>(new _Body(_args ...)); ret->m_this = ret; ret->setParent(_parent); return ret; }

//private:
	fRect m_geometry;	// parent coordinate system
	std::weak_ptr<ViewBody> m_this;		// weak_ptr to itself in order to gain access to its shared_ptr.
	std::weak_ptr<ViewBody> m_parent;
	std::set<View> m_children;
};

}
