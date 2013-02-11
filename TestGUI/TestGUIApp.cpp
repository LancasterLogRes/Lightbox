#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>
#define MONKSVG_USE_MONKVG
#include <openvg/mkOpenVG_SVG.h>
#include <Common/Global.h>
#include <Common/thread.h>
#include <GUI/TextLabel.h>
#include <GUI/Font.h>
#include "Global.h"
#include "TestGUIApp.h"
using namespace std;
using namespace Lightbox;

TestGUIApp::TestGUIApp()
{
	Lightbox::g_debugEnabled[SnoopGL] = 0;	// Enable/disable GL snoop.
}

static boost::shared_ptr<MonkSVG::OpenVG_SVGHandler> s_handler;
static VGPaint _paint;
static VGPath _path;

void TestGUIApp::initGraphics(Display& _d)
{
	GUIApp::initGraphics(_d);
	vgCreateContextMNK(GUIApp::joint().displaySizePixels.w(), GUIApp::joint().displaySizePixels.h(), VG_RENDERING_BACKEND_TYPE_OPENGLES20);

	// create a paint
	_paint = vgCreatePaint();
	vgSetPaint(_paint, VG_FILL_PATH );
	VGfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	vgSetParameterfv(_paint, VG_PAINT_COLOR, 4, &color[0]);

	// create a box path
	_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
	vguRect( _path, 50.0f, 50.0f, 90.0f, 50.0f );

	s_handler = boost::make_shared<MonkSVG::OpenVG_SVGHandler>();
	{
		MonkSVG::SVG s;
		s.initialize(s_handler);
		s.read(Resources::find("broken.svg").toString());
	}

//	_d.setAnimating();
}

bool TestGUIApp::drawGraphics()
{
	Super::drawGraphics();

//	glClearColor(0.f, 0.f, .25f, .25f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw SVG
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
//	vgScale(5, 5);
	vgTranslate(GUIApp::joint().display->width() / 2, GUIApp::joint().display->height() / 2);
	s_handler->draw();

	// Draw box
//	Context().rect(fRect(50, 50, 50, 50), Color(1.f, .25f));

	// Draw VG
//	vgSetPaint( _paint, VG_FILL_PATH );
//	vgDrawPath( _path, VG_FILL_PATH );

//	return true;
	m_mainView->child(0)->update();
	return false;
}

void TestGUIApp::finiGraphics(Display &_d)
{
	s_handler.reset();
	vgDestroyContextMNK();
}

void TestGUIApp::sendInitialEvents()
{
}

void TestGUIApp::go()
{
	Super::go();

	m_mainView = new MainView;
	m_mainView->setParent(root());
	root()->setLayout(new OverlayLayout);
}

void TestGUIApp::tick()
{
}
