/* BEGIN COPYRIGHT
 *
 * This file is part of Noted.
 *
 * Copyright ©2011, 2012, Lancaster Logic Response Limited.
 *
 * Noted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Noted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Noted.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <array>
#include <map>
#include "Global.h"
#include "Flags.h"

namespace lb
{

enum ColorSetItem
{
	ChromaToggle =			0x00000000,
	ChromaMin =				0x00000001,
	ChromaSelection =		0x00000002, // Use colorSelection() to determine actual set.
	ChromaContinuous =		0x00000008,
	ValueMax =				0x00000000,
	ValueToggle =			0x00000010,
	ValueSelection =		0x00000020,
	ValueContinuous =		0x00000080,
	HueSelection =			0x00000200, // Use colorSelection() to determine actual set.
	HueContinuous =			0x00000800,
	ColorSelection =		ChromaSelection | HueSelection,	// Use colorSelection() to determine actual set.
	SingleColor =			0x00001000	// Forces RGB to a value channel-wise <= to single color in selection.
};

LIGHTBOX_FLAGS(ColorSetItem, ColorSet, (ChromaToggle)(ChromaMin)(ChromaSelection)(ChromaContinuous)(ValueMax)(ValueToggle)(ValueSelection)(ValueContinuous)(HueSelection)(HueContinuous)(SingleColor));

static const ColorSet DimmableSelection =		HueSelection|ChromaSelection|ValueContinuous;
static const ColorSet DimmableMono =			SingleColor|ValueContinuous;
static const ColorSet FullColorSet =			HueContinuous|ChromaContinuous|ValueContinuous;

typedef std::pair<float, float> Range;
Range static const AutoRange = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};

template <class _N>
class XO
{
public:
	XO(_N _scale = 1, _N _offset = 0): m_scale(_scale), m_offset(_offset) {}
	static XO toUnity(Range _r) { return XO(1 / (_r.second - _r.first), -_r.first / (_r.second - _r.first)); }

	_N apply(_N _v) const { return _v * m_scale + m_offset; }
	Range apply(Range _v) const { return Range(apply(_v.first), apply(_v.second)); }
	template <class _T> void apply(std::vector<_T>& _v) const { for (auto& i: _v) i = apply(i); }

	_N scale() const { return m_scale; }
	_N offset() const { return m_offset; }

	bool isIdentity() const { return m_scale == 1 && m_offset == 0; }

	XO composed(XO const& _then) const { return XO(_then.m_scale * m_scale, _then.m_scale * m_offset + _then.m_offset); }

private:
	_N m_scale;
	_N m_offset;
};

typedef XO<float> XOf;

}
