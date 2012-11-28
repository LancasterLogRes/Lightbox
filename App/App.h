#pragma once

#include <boost/utility.hpp>
#include <Common/Global.h>
#include <Numeric/Coord.h>

class AInputEvent;

namespace Lightbox
{

class Display;

class App: boost::noncopyable
{
public:
	App() {}
	virtual ~App() {}

	virtual void initGraphics(Display&) {}
	virtual void drawGraphics(Display&) {}
	virtual foreign_vector<uint8_t const> state() { return foreign_vector<uint8_t const>(); }
	virtual void setState(foreign_vector<uint8_t> const&) {}

	virtual bool motionEvent(int, iCoord, bool) { return false; }
};

}
