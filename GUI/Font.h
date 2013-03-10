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

struct CharMetrics
{
	fRect glyph;
	fSize advance;
};

template <class _S> _S& operator<<(_S& _out, CharMetrics const& _m)
{
	_out << "CharMetrics(" << _m.glyph << _m.advance << ")";
	return _out;
}

typedef uint32_t Char;

class BakedFont;
typedef std::shared_ptr<BakedFont> BakedFontPtr;

enum class Metric
{
	Pixels,
	Millis
};

class Font
{
public:
	Font(): m_mm(0) {}
	Font(float _mm, std::string _family, bool _bold = false): Font(_mm, FontDefinition(_family, _bold)) {}
	Font(float _mm, FontDefinition const& _d): Font(Metric::Millis, _mm, _d) {}

	/// @warning If _m == Metric::Pixels, these won't work unless there's a display available.
	Font(Metric _m, float _u, std::string _family, bool _bold = false): Font(_m, _u, FontDefinition(_family, _bold)) {}
	Font(Metric _m, float _u, FontDefinition const& _d);

	BakedFontPtr getBaked() const;
	FontDefinition const& definition() const { return m_definition; }
	float mmSize() const { return m_mm; }

	void setBold(bool _bold) { m_definition.bold = _bold; m_baked.reset(); }
	void setMmSize(float _mm) { m_mm = _mm; m_baked.reset(); }

	void draw(fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black, AnchorType _t = AtCenter) const;
	void draw(iCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black, AnchorType _t = AtCenter) const;
	fRect measure(std::string const& _text, bool _tight = false) const;
	CharMetrics metrics(Char _char, Char _nChar, bool _tight = false) const;

	float pxSize() const;		///< @warning Won't work if there's no display.
	fRect pxMeasure(std::string const& _text, bool _tight = false) const;		///< @warning Won't work if there's no display.
	CharMetrics pxMetrics(Char _char, Char _nChar, bool _tight = false) const;	///< @warning Won't work if there's no display.

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
