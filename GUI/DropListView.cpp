#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Slate.h"
#include "DropListView.h"
using namespace std;
using namespace Lightbox;

DropListViewBody::DropListViewBody(ListModelPtr const& _model)
{
	setModel(_model);
}

DropListViewBody::~DropListViewBody()
{
}

bool DropListViewBody::event(Event* _e)
{
	return Super::event(_e);
}

void DropListViewBody::draw(Slate const& _c, unsigned _l)
{
	Super::draw(_c, _l);
	auto inner = rect().inset(innerMargin(effectiveGrouping()));
	if (_l == 0 && currentId() != UnknownModelId)
	{
		fSize s = model()->itemSize(index(currentId()), currentId());
		iSize sp = _c.toPixels(s);
		model()->drawItem(index(currentId()), currentId(), _c.sub(inner.lerpAlign(sp, 0, .5)), true);
	}
}

void DropListViewBody::tapped()
{
	if (!m_back)
	{
		m_back = ViewBody::create();
		m_back->setPrefill(Color(0, 0.75));
		m_back->setLayout(new HorizontalLayout);
		FrameBody::spawn(m_back)->withStretch(.5);
		m_list = ListViewBody::spawn(m_back);
		FrameBody::spawn(m_back)->withStretch(.5);
	}

	m_list->setModel(model());
	m_list->setCurrentId(currentId());
	m_back->setParent(GUIApp::root());
	m_list->setOnCurrentChanged([=](ListViewBody* l)
	{
		setCurrentId(l->currentId());
		m_back->setParent(nullptr);
	});

	Super::tapped();
}

