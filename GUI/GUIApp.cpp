#include <LGL/GL.h>
#include <Common/Global.h>
#include <LGL.h>
#include <App.h>
#include "Fonts.h"
#include "Global.h"
#include "View.h"
#include "GUIApp.h"
using namespace std;
using namespace Lightbox;

void Style::generateColors(Color _fore)
{
	fore = _fore;
	back = Color(_fore.h(), _fore.sat() / 2.f, _fore.value() / 2.f);
	high = Color(_fore.h() + .5f, _fore.sat(), _fore.value());
}

GUIApp::GUIApp()
{
	m_root = ViewBody::create();
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

	LB_GL(glEnable, GL_CULL_FACE);
	LB_GL(glDisable, GL_DEPTH_TEST);
	LB_GL(glEnable, GL_BLEND);
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_joint.init(_d);
	m_style.regular = Font(ubuntu_r_ttf, 16.f);
	m_style.bold = Font(ubuntu_b_ttf, 16.f);
	m_style.fore = Color(RGB8Space, 0x86, 0x9f, 0xc3);
	m_style.back = Color(RGBSpace, .4f, .5f, .6f, 1.f);
	m_style.high = Color(RGB8Space, 0xf3, 0xa3, 0x19);
	m_root->setGeometry(fRect(0, 0, _d.width(), _d.height()));
}

void GUIApp::drawGraphics()
{
	LB_GL(glClearColor, 0, 0, 0, 1.0f);
	LB_GL(glClear, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	m_root->handleDraw(Context());
}

bool GUIApp::motionEvent(int _id, iCoord _pos, int _direction)
{
	TouchEvent* ev = nullptr;
	if (_direction > 0)
		ev = new TouchDownEvent(_id, _pos);
	else if (_direction < 0)
		ev = new TouchUpEvent(_id, _pos);
	else if (m_pointerLock[_id])
		ev = new TouchMoveEvent(_id, _pos);

	if (!ev)
		return false;

	bool ret = false;
	if (m_pointerLock[_id])
	{
		auto gp = m_pointerLock[_id]->parent()->globalPos();
		ev->local -= gp;
		ret = m_pointerLock[_id]->handleEvent(ev);
		ev->local += gp;
	}
	if (!ret)
		ret = m_root->handleEvent(ev);

	delete ev;

	if (_direction < 0)
		m_pointerLock[_id] = nullptr;

	return ret;
}

GUIApp* GUIApp::get()
{
	return dynamic_cast<GUIApp*>(AppEngine::get()->app());
}

bool GUIApp::lockPointer(int _id, View const& _v)
{
	if (m_pointerLock[_id] == nullptr)
	{
		m_pointerLock[_id] = _v;
		return true;
	}
	return false;
}

bool GUIApp::releasePointer(int _id, View const& _v)
{
	if (m_pointerLock[_id] == _v)
	{
		m_pointerLock[_id] = nullptr;
		return true;
	}
	return false;
}
