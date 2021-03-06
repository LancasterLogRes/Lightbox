#pragma once

#include <boost/utility.hpp>
#include <Common/Global.h>
#include <Common/Time.h>
#include <Numeric/Coord.h>
#include "Global.h"

class AInputEvent;

namespace lb
{

class Display;

class App: boost::noncopyable
{
public:
	App() {}
	virtual ~App() {}

	virtual void initGraphics(Display&) {}
	virtual bool drawGraphics() { return true; }
	virtual void finiGraphics(Display&) {}

	virtual void go() {}
	virtual void sendInitialEvents() {}

	virtual void tick() {}
	virtual void iterate(Time) {}

	virtual foreign_vector<uint8_t const> state() { return foreign_vector<uint8_t const>(); }
	virtual void setState(foreign_vector<uint8_t> const&) {}

	virtual bool motionEvent(int, iCoord, int) { return false; }
	virtual bool keyEvent(int, int) { return false; }
};

}
