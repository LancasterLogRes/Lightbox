#pragma once

#include <Numeric/Coord.h>

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

struct BatteryEvent: public Event
{
	BatteryEvent(int _level): level(_level) {}
	int level;
};

}


