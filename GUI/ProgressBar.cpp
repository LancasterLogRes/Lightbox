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

void ProgressBarBody::draw(Context const& _c, unsigned)
{
	_c.rect(rect(), GUIApp::style().back, 0.1);
	iRect inner = rect().inset(GUIApp::joint().display->toPixels(fSize(5, 5)));
	_c.rect(inner.lerp(0, 0, m_progress, 1), GUIApp::style().high, -0.2);
}


