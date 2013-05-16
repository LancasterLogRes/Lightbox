#pragma once

#include "View.h"

namespace lb
{

class ProgressBarBody;
typedef boost::intrusive_ptr<ProgressBarBody> ProgressBar;

class ProgressBarBody: public ViewCreator<ViewBody, ProgressBarBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, ProgressBarBody> Super;
	virtual ~ProgressBarBody();

	float progress() const { return m_progress; }

	void setProgress(float _p) { m_progress = _p; update(); }
	
protected:
	ProgressBarBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);
	
private:
	float m_progress;
};

}


