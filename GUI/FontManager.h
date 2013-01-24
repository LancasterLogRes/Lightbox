#pragma once

#include <map>
#include <memory>
#include "Font.h"
#include "BakedFont.h"

namespace Lightbox
{

class FontManager
{
public:
	explicit FontManager(bool _canRender = false);
	~FontManager();

	void initGraphics() { m_canRender = true; }
	void finiGraphics() { m_rendered.clear(); m_canRender = false; }

	void registerData(FontDefinition const& _def, foreign_vector<uint8_t const> const& _data) { m_data[_def] = _data; }
	void unregisterData(FontDefinition const& _def) { m_data.erase(_def); }
	BakedFontPtr getBaked(Font const& _f);
	
private:
	std::map<Font, BakedFontPtr> m_rendered;
	std::map<FontDefinition, foreign_vector<uint8_t const> > m_data;
	bool m_canRender;
};

}


