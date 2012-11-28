#pragma once

#include <App/App.h>
#include "View.h"

namespace Lightbox
{

class GUIApp: public App
{
public:
	GUIApp();
	virtual ~GUIApp();

	static GUIApp* get();
	View const& root() { return m_root; }

	virtual void initGraphics(Display& _d);
	virtual void drawGraphics(Display& _d);
	virtual bool motionEvent(int _id, iCoord _pos, bool _isNew);

private:
	View m_root;
};

}


