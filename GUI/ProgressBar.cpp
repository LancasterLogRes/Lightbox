#include <Common/Global.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "ProgressBar.h"
using namespace std;
using namespace Lightbox;

ProgressBarBody::ProgressBarBody():
	m_progress(0)
{
}

ProgressBarBody::~ProgressBarBody()
{
}

bool ProgressBarBody::event(Event* _e)
{
	return Super::event(_e);
}

void ProgressBarBody::draw(Context const& _c)
{
	iRect r(_c.pixels(this));
	_c.rect(r, GUIApp::style().back, 0.1);
	iRect t(r.shrunk(min(r.w(), r.h()) / 10));
	t.setWidth(m_progress * t.w());
	_c.rect(t, GUIApp::style().high, -0.2);
}


