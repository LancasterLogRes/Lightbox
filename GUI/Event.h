#pragma once

#include <Common/Time.h>
#include <Numeric/Coord.h>

namespace lb
{

struct Event
{
	virtual ~Event() {}
};

struct TouchEvent: public Event
{
	TouchEvent(int _id, fCoord _local, iCoord _global): id(_id), m_mmLocal(_local), global(_global) {}

	iCoord local() const;
	fCoord mmLocal() const { return m_mmLocal; }

	int id;
	fCoord m_mmLocal;
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

struct IterateEvent: public Event
{
	Time delta;
};

}


