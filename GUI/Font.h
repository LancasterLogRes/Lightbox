#pragma once

#include <memory>
#include <Common/Pimpl.h>
#include <Common/Global.h>
#include <Common/RGBA.h>
#include <Numeric/Coord.h>
#include "Global.h"

namespace Lightbox
{

LIGHTBOX_STRUCT(2, FontDefinition, std::string, family, bool, bold);

class BakedFont;
typedef std::shared_ptr<BakedFont> BakedFontPtr;

class Font
{
public:
	Font(): m_mm(0) {}
	Font(float _mm, std::string _family, bool _bold = false): Font(_mm, FontDefinition(_family, _bold)) {}
	Font(float _mm, FontDefinition const& _d): m_definition(_d), m_mm(_mm) {}

	BakedFontPtr getBaked() const;
	FontDefinition const& definition() const { return m_definition; }
	float mmSize() const { return m_mm; }

	void draw(fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;
	void draw(iCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;
	fSize measure(std::string const& _text) const;
	iSize measurePx(std::string const& _text) const;

	bool isValid() const { return m_mm > 0; }
	bool isNull() const { return m_mm == 0; }
	bool operator<(Font const& _cmp) const { return m_definition < _cmp.m_definition || (m_definition == _cmp.m_definition && m_mm < _cmp.m_mm); }
	bool operator==(Font const& _cmp) const { return m_definition == _cmp.m_definition && m_mm == _cmp.m_mm; }
	bool operator!=(Font const& _cmp) const { return !operator==(_cmp); }

private:
	FontDefinition m_definition;
	float m_mm;
	mutable std::weak_ptr<BakedFont> m_baked;
};

}
