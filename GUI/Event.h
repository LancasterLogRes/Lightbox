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
	TouchEvent(int _id, fCoord _local, iCoord _global): id(_id), local(_local), global(_global) {}
	int id;
	fCoord local;
	iCoord global;
};

struct TouchDownEvent: public TouchEvent { TouchDownEvent(int _id, fCoord _local, iCoord _global): TouchEvent(_id, _local, _global) {} };
struct TouchUpEvent: public TouchEvent { TouchUpEvent(int _id, fCoord _local, iCoord _global): TouchEvent(_id, _local, _global) {} };
struct TouchMoveEvent: public TouchEvent { TouchMoveEvent(int _id, fCoord _local, iCoord _global): TouchEvent(_id, _local, _global) {} };

struct BatteryEvent: public Event
{
	BatteryEvent(int _level): level(_level) {}
	int level;
};

}


