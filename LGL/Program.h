#pragma once

#include <memory>
#include <functional>
#include <boost/utility.hpp>
#include <Common/Pimpl.h>
#include <Common/Algorithms.h>
#include "Uniform.h"
#include "Metrics.h"
#include "Attrib.h"
#include "ProgramFace.h"
#include "Global.h"

namespace Lightbox
{

class Program: public Pimpl<ProgramFace>
{
	friend class ProgramFace;
	friend class ProgramUser;

public:
	Program() {}
	Program(Shader const& _vs, Shader const& _fs):
		Pimpl<ProgramFace>(new ProgramFace)
	{
		m_p->setVertex(_vs);
		m_p->setFragment(_fs);
		m_p->link();
	}

	NonVolatile operator[](std::string const& _name) const { return uniform(_name).keep(); }
	Attrib attrib(std::string const& _name) const { return Attrib(*this, _name); }
	Uniform uniform(std::string const& _name) const { return Uniform(*this, _name); }

	void tie(UniformPage const& _p) { if (m_p) m_p->tie(_p); }
	void untie(UniformPage const& _p) { if (m_p) m_p->untie(_p); }

	static Program inUse() { return Program(ProgramFace::inUse()); }

private:
	Program(std::shared_ptr<ProgramFace> _p): Pimpl<ProgramFace>(_p) {}

	void use() const { if (m_p) m_p->use(m_p); }
	void drop() const { if (m_p) m_p->drop(); }
};

class ProgramUser: public boost::noncopyable
{
public:
	ProgramUser(): m_p(ProgramFace::inUse()), m_owns(false) { assert(m_p); }
	ProgramUser(Program const& _p): m_p(_p ? _p : ProgramFace::inUse()), m_owns(!!_p) { if (m_owns) { assert(!ProgramFace::inUse()); m_p.use(); } else assert(m_p); }
	~ProgramUser() { if (m_owns) m_p.drop(); }

	Attrib attrib(std::string const& _name) const { return Attrib(m_p, _name); }
	Uniform uniform(std::string const& _name) const { return Uniform(m_p, _name); }

	void points(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_POINTS, _first, _count); g_metrics.incDraw(); }
	void lines(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_LINES, _first, _count); g_metrics.incDraw(); }
	void lineLoop(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_LINE_LOOP, _first, _count); g_metrics.incDraw(); }
	void lineStrip(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_LINE_STRIP, _first, _count); g_metrics.incDraw(); }
	void triangles(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_TRIANGLES, _first, _count); g_metrics.incDraw(); }
	void triangleStrip(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_TRIANGLE_STRIP, _first, _count); g_metrics.incDraw(); }
	void triangleFan(GLsizei _count, GLint _first = 0) { LB_GL(glDrawArrays, GL_TRIANGLE_FAN, _first, _count); g_metrics.incDraw(); }

private:
	Program m_p;
	bool m_owns;
};

class PagedUniform;

class UniformPageFace
{
public:
	UniformPageFace() {}

	inline PagedUniform const& get(std::string const& _name);
	inline void registerProgram(ProgramFace* _p);
	inline void unregisterProgram(ProgramFace* _p);
	inline void registerName(std::string const& _n);
	inline void setup(std::shared_ptr<ProgramFace> const& _p);

private:
	std::map<std::string, PagedUniform> m_items;
	std::set<ProgramFace*> m_programs;
};

class PagedUniformFace
{
public:
	PagedUniformFace() {}

	inline void registerProgram(std::string const& _thisName, ProgramFace* _p);
	inline void unregisterProgram(ProgramFace* _p);
	inline void setup(std::shared_ptr<ProgramFace> const& _p);

	void set(std::function<void(Uniform const&)> const& _f) { m_f = _f; }

private:
	std::function<void(Uniform const&)> m_f;
	std::map<ProgramFace*, int> m_locations;
};

class PagedUniform: public Pimpl<PagedUniformFace>, public UniformSetter<PagedUniform>
{
public:
	PagedUniform(): Pimpl(std::make_shared<PagedUniformFace>()) {}
	void set(Texture2D const& _t) const { setNV(_t); }
	void set(fMatrix4 const& _m4x4) const { setNV(_m4x4); }

	using UniformSetter::operator=;

	template <class ... Params> void set(Params ... _p) const { setNV(_p ...); if (ProgramFace::inUse()) m_p->setup(ProgramFace::inUse()); }

//	template <class ... Params> void setNV(Params ... _p) { sharedPtr()->set(std::make_shared<std::function<void()> >([=](Uniform const& u){ u.set(_p ...); })); }
	template <class T> void setNV(T _0) const { sharedPtr()->set(std::function<void(Uniform const&)>([=](Uniform const& u){ u.set(_0); })); }
	template <class T> void setNV(T _0, T _1) const { sharedPtr()->set(std::function<void(Uniform const&)>([=](Uniform const& u){ u.set(_0, _1); })); }
	template <class T> void setNV(T _0, T _1, T _2) const { sharedPtr()->set(std::function<void(Uniform const&)>([=](Uniform const& u){ u.set(_0, _1, _2); })); }
	template <class T> void setNV(T _0, T _1, T _2, T _3) const { sharedPtr()->set(std::function<void(Uniform const&)>([=](Uniform const& u){ u.set(_0, _1, _2, _3); })); }
};

static const PagedUniform NullPagedUniform;

class UniformPage: public Pimpl<UniformPageFace>
{
public:
	UniformPage(): Pimpl(std::make_shared<UniformPageFace>()) {}
	PagedUniform const& operator[](std::string const& _name)
	{
		if (m_p)
			return m_p->get(_name);
		else
			return NullPagedUniform;
	}
};

PagedUniform const& UniformPageFace::get(std::string const& _name)
{
	if (!m_items.count(_name))
		registerName(_name);
	return m_items[_name];
}

void UniformPageFace::registerProgram(ProgramFace* _p)
{
	for (auto& i: m_items)
		i.second.sharedPtr()->registerProgram(i.first, _p);
	m_programs.insert(_p);
}

void UniformPageFace::unregisterProgram(ProgramFace* _p)
{
	for (auto& i: m_items)
		i.second.sharedPtr()->unregisterProgram(_p);
	m_programs.erase(_p);
}

void UniformPageFace::registerName(std::string const& _n)
{
	auto& item = m_items[_n];
	for (auto& i: m_programs)
		item.sharedPtr()->registerProgram(_n, i);
}

void UniformPageFace::setup(std::shared_ptr<ProgramFace> const& _p)
{
	if (Assert(m_programs.count(&*_p)))
	{
		for (auto& i: m_items)
			i.second.sharedPtr()->setup(_p);
	}
}

void PagedUniformFace::registerProgram(std::string const& _thisName, ProgramFace* _p)
{
	auto it = m_locations.find(_p);
	if (it == m_locations.end())
	{
		int l = _p->uniformLocation(_thisName);
		if (l > -1)
			m_locations[_p] = l;
	}
}

void PagedUniformFace::unregisterProgram(ProgramFace* _p)
{
	auto it = m_locations.find(_p);
	if (it != m_locations.end())
		m_locations.erase(it);
}

void PagedUniformFace::setup(std::shared_ptr<ProgramFace> const& _p)
{
	if (m_f)
	{
		auto it = m_locations.find(&*_p);
		if (it != m_locations.end())
			m_f(Uniform(_p, it->second));
	}
}

}
