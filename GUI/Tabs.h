#pragma once

#include <map>
#include <string>

#include "View.h"

namespace Lightbox
{

class TabsBody;
typedef boost::intrusive_ptr<TabsBody> Tabs;

class TabsBody: public ViewCreator<ViewBody, TabsBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, TabsBody> Super;
	~TabsBody();

	void addTab(std::string const& _title, View const& _page);
	void focusTab(View const& _page);
	
protected:
	TabsBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);
	
private:
	using ViewCreator::setLayout;
	using ViewCreator::layout;
};

}


