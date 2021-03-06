#pragma once

#include <map>
#include <memory>
#include "Font.h"
#include "BakedFont.h"

namespace lb
{

class FontManager
{
public:
	explicit FontManager(bool _canRender = false);
	~FontManager();

	void initGraphics() { m_canRender = true; }
	void finiGraphics() { m_rendered.clear(); m_canRender = false; }

	void registerData(FontDefinition const& _def, foreign_vector<uint8_t const> const& _data) { m_info[_def] = FontInfo(_data); }
	void unregisterData(FontDefinition const& _def) { m_info.erase(_def); }
	BakedFontPtr getBaked(Font const& _f);
	FontInfo getInfo(FontDefinition const& _f) { return m_info.at(_f); }

private:
	std::map<Font, BakedFontPtr> m_rendered;
	std::map<FontDefinition, FontInfo> m_info;

	bool m_canRender;
};

}


