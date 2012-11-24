#pragma once

#include <Common/Global.h>

class AInputEvent;

namespace Lightbox
{

class Display;

class App
{
public:
	App();
	virtual ~App();

	virtual void initGraphics(Display&) {}
	virtual void drawGraphics(Display&) {}
	virtual foreign_vector<uint8_t const> state() { return foreign_vector<uint8_t const>(); }
	virtual void setState(foreign_vector<uint8_t> const&) {}
	virtual bool motionEvent(uint32_t, uint32_t, AInputEvent*) { return false; }

private:
	App(App const&) = delete;
	App& operator=(App const&) = delete;
};

}
