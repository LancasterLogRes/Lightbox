#include <Common/Global.h>

#undef _S
#undef _T

namespace Lightbox
{

template<class T> class Pair;

template<class _S, class _T> _S& operator>>(_S& _in, Pair<_T>& _me);
template<class _S, class _T> _S& operator<<(_S& _out, Pair<_T> const& _me);

template<class T>
class Pair
{
	typedef Pair<T> This;

public:
	Pair() {}
	Pair(T _x, T _y) : m_x(_x), m_y(_y) {}
	Pair(T const* _data) : m_x(_data[0]), m_y(_data[1]) {}
	template<typename _T> explicit Pair(_T const* _data) : m_x(_data[0]), m_y(_data[1]) {}
	template<typename _T> explicit Pair(Pair<_T> _data) : m_x(_data.x()), m_y(_data.y()) {}

	This& asPair() { return *this; }
	This const& asPair() const { return *this; }

	This& sum(T _n) { m_x += _n; m_y += _n; return *this; }
	This& sub(T _n) { m_x -= _n; m_y -= _n; return *this; }
	This& scale(T _n) { m_x *= _n; m_y *= _n; return *this; }
	This& slash(T _n) { m_x /= _n; m_y /= _n; return *this; }

	This& sum(This _n) { m_x += _n.m_x; m_y += _n.m_y; return *this; }
	This& sub(This _n) { m_x -= _n.m_x; m_y -= _n.m_y; return *this; }
	This& scale(This _n) { m_x *= _n.m_x; m_y *= _n.m_y; return *this; }
	This& slash(This _n) { m_x /= _n.m_x; m_y /= _n.m_y; return *this; }

	This summed(T _n) const { return This(m_x + _n, m_y + _n); }
	This subbed(T _n) const { return This(m_x - _n, m_y - _n); }
	This scaled(T _n) const { return This(m_x * _n, m_y * _n); }
	This slashed(T _n) const { return This(m_x / _n, m_y / _n); }

	This summed(This _n) const { return This(m_x + _n.m_x, m_y + _n.m_y); }
	This subbed(This _n) const { return This(m_x - _n.m_x, m_y - _n.m_y); }
	This scaled(This _n) const { return This(m_x * _n.m_x, m_y * _n.m_y); }
	This slashed(This _n) const { return This(m_x / _n.m_x, m_y / _n.m_y); }

	void setX(T _x) { m_x = _x; }
	void setY(T _y) { m_y = _y; }
	template<typename _T> void set(_T _x, _T _y) { m_x = _x; m_y = _y; }

	T x() const { return m_x; }
	T y() const { return m_y; }
	T const* data() const { return &m_x; }

	bool compare(This _c) const { return m_x == _c.m_x && m_y == _c.m_y; }

	T length() const { return sqrt(lengthSquared()); }
	T lengthSquared() const { return x()*x() + y()*y(); }

	T min() const { return std::min(m_x, m_y); }
	T max() const { return std::max(m_x, m_y); }
	This& clamp(T _min, T _max) { m_x = (std::min(std::max((m_x, _min), _max))); m_y = (std::min(std::max(m_y, _min), _max)); return *this; }
	This clamped(T _min, T _max) const { return This(std::min(std::max(m_x, _min), _max), std::min(std::max(m_y, _min), _max)); }
	This sign() const { return This(m_x > 0 ? 1 : m_x < 0 ? -1 : 0, m_y > 0 ? 1 : m_y < 0 ? -1 : 0); }
	This rounded() const { return This(round(m_x), round(m_y)); }

protected:
	T m_x;
	T m_y;
};

template<class T, typename R>
class CalcPair: public Pair<T>
{
public:
	typedef Pair<T> Super;

	CalcPair() {}
	CalcPair(T _x, T _y): Pair<T>(_x, _y) {}
	CalcPair(T const* _data): Pair<T>(_data) {}
	template<typename _T> explicit CalcPair(_T const* _data): Pair<T>(_data) {}
	template<typename _T> explicit CalcPair(Pair<_T> const& _data): Pair<T>(_data) {}

	using Super::summed;
	using Super::subbed;
	using Super::scaled;
	using Super::slashed;
	using Super::sum;
	using Super::scale;
	using Super::sub;
	using Super::slash;
	using Super::min;
	using Super::max;

	R& clamp(T _min, T _max) { return Super::clamp(_min, _max); }
	R clamped(T _min, T _max) const { return Super::clamped(_min, _max); }
	R sign() const { return Super::sign(); }
	R rounded() const { return Super::rounded(); }

	R operator+(R _x) const { return R(summed(_x)); }
	R operator-(R _x) const { return R(subbed(_x)); }
	R operator*(R _x) const { return R(scaled(_x)); }
	R operator/(R _x) const { return R(slashed(_x)); }
	R operator+(T _x) const { return R(summed(_x)); }
	R operator-(T _x) const { return R(subbed(_x)); }
	R operator*(T _x) const { return R(scaled(_x)); }
	R operator/(T _x) const { return R(slashed(_x)); }

	R operator-() const { return R(scaled(T(-1))); }

	inline friend R operator*(T _a, R _b) { return R(_b.scaled(_a)); }
	inline friend R operator/(T _a, R _b) { return R(_b.slashed(_a)); }

	R& operator+=(R _x) { sum(_x); return *(R*)this; }
	R& operator-=(R _x) { sub(_x); return *(R*)this; }
	R& operator*=(R _x) { scale(_x); return *(R*)this; }
	R& operator/=(R _x) { slash(_x); return *(R*)this; }
	R& operator+=(T _x) { sum(_x); return *(R*)this; }
	R& operator-=(T _x) { sub(_x); return *(R*)this; }
	R& operator*=(T _x) { scale(_x); return *(R*)this; }
	R& operator/=(T _x) { slash(_x); return *(R*)this; }
};

template<class _S, class _T> _S& operator>>(_S& _in, Pair<_T>& _me)
{
	while (_in && _in.get() != '(') {}
	_in >> _me.m_x;
	if (_in.get() == ',')
		_in >> _me.m_y;
	_in.get();
	return _in;
}

template<class _S, class _T> _S& operator<<(_S& _out, Pair<_T> const& _me)
{
	_out << "(" << _me.x() << ", " << _me.y() << ")";
	return _out;
}

}
