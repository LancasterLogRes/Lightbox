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

struct Context
{
	fRect clip;
};

class ViewBody;
typedef std::shared_ptr<ViewBody> View;

class ViewBody: public boost::noncopyable
{
public:
	template <class ... _T> static View create(_T ... _args) { return View(new ViewBody(_args ...)); }
	virtual ~ViewBody();

	void setParent(View const& _p);
	void setGeometry(iRect _geometry) { m_geometry = _geometry; resized(); }

	virtual void draw(Context _c);
//	virtual void event();
	virtual void resized() {}

//protected:
//	virtual MemberMap propertyMap() const { return MemberMap(); }

protected:
	ViewBody(View const& _parent = nullptr) { setParent(_parent); }

//private:
	iRect m_geometry;	// parent coordinate system
	std::weak_ptr<ViewBody> m_parent;
	std::set<View> m_children;
};

}
