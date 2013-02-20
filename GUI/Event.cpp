#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Event.h"
using namespace std;
using namespace Lightbox;

iCoord TouchEvent::local() const
{
	return GUIApp::joint().display->toPixels(m_mmLocal);
}
