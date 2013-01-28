#pragma once

#include <memory>
#include <Common/Pimpl.h>
#include <Common/Global.h>
#include <Common/RGBA.h>
#include <Common/Flags.h>
#include <Numeric/Rect.h>
#include "Global.h"

namespace Lightbox
{

LIGHTBOX_STRUCT(2, FontDefinition, std::string, family, bool, bold);

enum AnchorFlags
{
	AtCenter = 0,
	AtTop = 1,
	AtBottom = 2,
	AtBaseline = 4,
	AtLeft = 16,
	AtRight = 32,
	TightVertical = 256
};

LIGHTBOX_FLAGS_TYPE(AnchorFlags, AnchorType);

class BakedFont;
typedef std::shared_ptr<BakedFont> BakedFontPtr;

enum Metric
{
	ByPixels = 0,
	ByMillis
};

class Font
{
public:
	Font(): m_mm(0) {}
	Font(float _mm, std::string _family, bool _bold = false): Font(_mm, FontDefinition(_family, _bold)) {}
	Font(float _mm, FontDefinition const& _d): Font(ByMillis, _mm, _d) {}
	Font(Metric _m, float _u, std::string _family, bool _bold = false): Font(_m, _u, FontDefinition(_family, _bold)) {}
	Font(Metric _m, float _u, FontDefinition const& _d);

	BakedFontPtr getBaked() const;
	FontDefinition const& definition() const { return m_definition; }
	float mmSize() const { return m_mm; }

	void draw(fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black, AnchorType _t = AtCenter) const;
	void draw(iCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black, AnchorType _t = AtCenter) const;
	fRect measure(std::string const& _text, bool _tight = false) const;
	fRect measurePx(std::string const& _text, bool _tight = false) const;

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
