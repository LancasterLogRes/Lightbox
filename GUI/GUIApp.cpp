#include <GLES2/gl2.h>
#include <Common/Global.h>
#include <LGL.h>
#include <App.h>
#include "Global.h"
#include "View.h"
#include "GUIApp.h"
using namespace std;
using namespace Lightbox;

GUIApp::GUIApp()
{
	m_root = ViewBody::create(nullptr);
}

GUIApp::~GUIApp()
{
}

void GUIApp::initGraphics(Display& _d)
{
	cnote << "Version" << (char const*)glGetString(GL_VERSION);
	cnote << "Vendor" << (char const*)glGetString(GL_VENDOR);
	cnote << "Renderer" << (char const*)glGetString(GL_RENDERER);
	cnote << "Extensions" << (char const*)glGetString(GL_EXTENSIONS);

	LIGHTBOX_GL(glEnable(GL_CULL_FACE));
	LIGHTBOX_GL(glDisable(GL_DEPTH_TEST));
	LIGHTBOX_GL(glEnable(GL_BLEND));
	LIGHTBOX_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	FontFace::initForDisplay(_d.size());
	ViewManager::init(_d);

	m_root->setGeometry(fRect(0, 0, _d.width(), _d.height()));
}

void GUIApp::drawGraphics(Display& _d)
{
	LIGHTBOX_GL(glClearColor(0, 0, 0, 1.0f));
	LIGHTBOX_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
	m_root->handleDraw(Context());
}

bool GUIApp::motionEvent(int _id, iCoord _pos, bool _isNew)
{
	Event* ev;
	if (_isNew)
		ev = new TouchDownEvent(_id, _pos);
	else if (_pos == iCoord(-1, -1))
		ev = new TouchUpEvent(_id, _pos);
	else
		ev = new TouchMoveEvent(_id, _pos);
	bool ret = m_root->handleEvent(ev);
	delete ev;
	return ret;
}

GUIApp* GUIApp::get()
{
	return dynamic_cast<GUIApp*>(AppEngine::get()->app());
}
