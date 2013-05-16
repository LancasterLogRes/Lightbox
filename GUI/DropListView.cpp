#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Slate.h"
#include "DropListView.h"
using namespace std;
using namespace lb;

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
	if (_l == 0 && selected() != UnknownModelId)
	{
		fSize s = model()->itemSize(index(selected()), selected());
		iSize sp = _c.toPixels(s);
		model()->drawItem(index(selected()), selected(), _c.sub(inner.lerpAlign(sp, 0, .5)), true);
	}
}

void DropListViewBody::tapped()
{
	if (!m_back)
	{
		m_back = SensitiveViewBody::create();
		m_back->setPrefill(Color(0, 0.5));
		m_back->setLayout(new HorizontalLayout);
		FrameBody::spawn(m_back)->withStretch(.5);
		m_list = ListViewBody::spawn(m_back);
		FrameBody::spawn(m_back)->withStretch(.5);
	}

	m_list->setModel(model());
	m_list->setSelected(selected());
	m_list->ensureVisible(selected());
	m_back->setParent(GUIApp::root());
	m_back->setOnTapped([=](SensitiveView)
	{
		m_back->setParent(nullptr);
	});
	m_list->setNotifyOnReselect(true);
	m_list->setOnSelectionChanged([=](ListView l)
	{
		setSelected(l->selected());
		m_back->setParent(nullptr);
	});

	Super::tapped();
}

