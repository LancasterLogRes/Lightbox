#pragma once

#include <functional>
#include <Common/Flags.h>
#include <Common/Color.h>
#include <LGL/Texture2D.h>
#include <GUI/Slate.h>
#include <GUI/GUIApp.h>
#include <GUI/RenderToTextureSlate.h>
#include "Font.h"
#include "View.h"

namespace Lightbox
{

class BasicButtonBody;
typedef boost::intrusive_ptr<BasicButtonBody> BasicButton;

void drawButton(Slate const& _c, iRect _inner, Color _color, bool _down, bool _base, bool _lit, bool _polish, bool _border = true);
void drawBorder(Slate const& _con, iRect _inner, bool _base, bool _lit, Color _col);
iMargin borderMargin();
iMargin innerMargin(Grouping _grouping);

class BasicButtonBody: public ViewCreator<ViewBody, BasicButtonBody>
{
	friend class ViewBody;

public:
	virtual ~BasicButtonBody() {}

	static Layers layers();

	bool isDown() const { return m_isDown; }
	bool isLit() const { return m_isLit; }
	Color color() const { return m_color; }
	Grouping grouping() const { return m_grouping; }
	Grouping effectiveGrouping() const;

	void setOnTapped(EventHandler const& _t) { m_onTapped = _t; }
	void setColor(Color _c) { m_color = _c; update(); }
	void setGrouping(Grouping _g) { m_grouping = _g; update(); }

	void setDown(bool _down = true);
	void setLit(bool _lit = true);

	BasicButton withOnTapped(EventHandler const& _t) { setOnTapped(_t); return this; }
	BasicButton withColor(Color _c) { setColor(_c); return this; }
	BasicButton withGrouping(Grouping _g) { setGrouping(_g); return this; }

protected:
	explicit BasicButtonBody(Color _c = White, Grouping _grouping = NoGrouping);

	virtual void draw(Slate const& _c, unsigned _layer);
	virtual bool event(Event* _e);
	virtual void tapped();
	virtual void pushed() { setDown(); setLit(); }
	virtual void released(bool _withFinger) { setDown(false); setLit(false); if (_withFinger) tapped(); }
	virtual void initGraphics();
	virtual void enabledChanged();
	virtual void updateLayers() { layer(1).show(isEnabled() && isLit()); }

private:
	EventHandler m_onTapped;
	Color m_color;
	Grouping m_grouping;

	bool m_isLit;
	bool m_isDown;
	int m_downPointer;
};

template <class _T> inline _T const& operator<<(_T const& _t, BasicButton const& _a) { _a->setOnTapped(_t); return _t; }

template <class _ButtonBody, class _FinalClass>
class TextualButtonCreator: public ViewCreator<_ButtonBody, _FinalClass>
{
	friend class ViewBody;
	typedef typename ViewCreator<_ButtonBody, _FinalClass>::Super Super;
	typedef typename ViewCreator<_ButtonBody, _FinalClass>::This This;

public:
	std::string const& text() const { return m_text; }

	void setText(std::string const& _s) { m_text = _s; updateTexture(); Super::update(); }

	This withText(std::string const& _s) { setText(_s); return this; }

protected:
	TextualButtonCreator(): _ButtonBody() {}
	template <class ... _P> explicit TextualButtonCreator(std::string const& _text, _P ... _args): ViewCreator<_ButtonBody, _FinalClass>(_args ... ), m_text(_text) {}
	template <class ... _P> explicit TextualButtonCreator(std::string const& _text, Font const& _f, _P ... _args): ViewCreator<_ButtonBody, _FinalClass>(_args ... ), m_text(_text), m_font(_f) {}

	virtual void draw(Slate const& _c, unsigned _l)
	{
		Super::draw(_c, _l);
		auto inner = Super::rect().inset(innerMargin(Super::effectiveGrouping()));
		if (_l == 0)
			_c.text(m_font.isValid() ? m_font : Super::isDown() ? GUIApp::style().bold : GUIApp::style().regular, inner.lerp(.5f, .5f), boost::algorithm::to_upper_copy(m_text), Color(Super::color().hue(), Super::color().sat() * .75f, Super::color().value() * .75f));
		if (_l == 1)
		{
			ProgramUser u(GUIApp::joint().colorize);
			u.uniform("u_amplitude") = 2.f;
			u.uniform("u_overglow") = 1.f;
			u.uniform("u_color") = (fVector4)Super::color().toRGBA();
			_c.blit(m_glowText, inner.lerp(.5f, .5f) - (iSize)m_glowText.size() / 2);
		}
	}

	virtual void initGraphics()
	{
		updateTexture();
		Super::initGraphics();
	}

	virtual void finiGraphics()
	{
		m_glowText = Texture2D();
		Super::finiGraphics();
	}

	virtual fSize specifyMinimumSize(fSize) const
	{
		// Until Fonts work without renderer.
		return GUIApp::get() ? GUIApp::style().bold.measure(m_text).size() : fSize(0, 0);
	}

private:
	void updateTexture()
	{
		if (m_text.size())
		{
			Font f = m_font.isValid() ? m_font : GUIApp::style().bold;
			std::string t = boost::algorithm::to_upper_copy(m_text);
			Texture2D baseText(iSize(f.pxMeasure(t).size()) + (iSize)GUIApp::joint().glowPixels * 2);
			{
				RenderToTextureSlate c(baseText);
				c.text(f, iCoord(baseText.size() / 2), t, Color(1.f / (GUIApp::joint().glowLevels * 2 + 1)));
			}
			m_glowText = GUIApp::joint().makeGlowerNear(baseText);
		}
	}

	std::string m_text;
	Font m_font;

	Texture2D m_glowText;
};

class TextButtonBody: public TextualButtonCreator<BasicButtonBody, TextButtonBody>
{
public:
	template <class ... _P> TextButtonBody(_P ... _args): TextualButtonCreator(_args ... ) {}
};
typedef boost::intrusive_ptr<TextButtonBody> TextButton;

}
