#pragma once

#include <memory>

namespace lb
{

template<class T>
class Pimpl
{
public:
	Pimpl(std::shared_ptr<T> const& _p = std::shared_ptr<T>()): m_p(_p) {}

	explicit operator bool() const { return isValid(); }
	bool isValid() const { return !!m_p; }
	bool isNull() const { return !m_p; }

	bool operator==(Pimpl<T> const& _c) const { return _c.m_p == m_p; }
	bool operator!=(Pimpl<T> const& _c) const { return _c.m_p != m_p; }

	std::shared_ptr<T> const& sharedPtr() const { return m_p; }

protected:
	explicit Pimpl(T* _p): m_p(std::shared_ptr<T>(_p)) {}

	std::shared_ptr<T> m_p;
};

}
