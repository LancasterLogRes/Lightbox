#pragma once

#include <App/App.h>
#include <Common/Color.h>
#include "Joint.h"
#include "View.h"

namespace Lightbox
{

struct Style
{
	void generateColors(Color _fore);
	Color back;		// usually similar hue to fore, but dark and desaturated.
	Color fore;		// usually fairly bright and vibrant.
	Color high;		// usually complimentary to fore.
	Font regular;
	Font bold;
};

class GUIApp: public App
{
public:
	GUIApp();
	virtual ~GUIApp();

	static GUIApp* get();
	static Joint& joint() { return get()->m_joint; }
	static View const& root() { return get()->m_root; }
	static Style const& style() { return get()->m_style; }

	virtual void initGraphics(Display& _d);
	virtual void drawGraphics();
	virtual bool motionEvent(int _id, iCoord _pos, int _direction);

	bool lockPointer(int _id, View const& _v);
	bool releasePointer(int _id, View const& _v);
	bool pointerLocked(int _id, View const& _v) const;

private:
	View m_root;
	Joint m_joint;
	Style m_style;

	std::array<View, s_maxPointers> m_pointerLock;
};

}


