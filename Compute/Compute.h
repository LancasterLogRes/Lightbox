#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <Common/GraphMetadata.h>
#include "Common.h"

namespace lb
{

class GenericComputeImpl
{
public:
	GenericComputeImpl() {}
	virtual ~GenericComputeImpl() {}
	virtual void init() {}
	virtual char const* name() const { return ""; }	// should be = 0;
	virtual SimpleKey hash() const { return 0; }
	virtual char const* elementTypeName() const = 0;
	virtual size_t elementSize() const = 0;
	virtual void genericCompute(std::vector<uint8_t>& _v) = 0;
};

class GenericCompute
{
public:
	GenericCompute() {}
	GenericCompute(GenericComputeImpl* _p): m_p(_p) {}
	GenericCompute(GenericCompute const& _p): m_p(_p.m_p) {}

	explicit operator bool() const { return !!m_p; }

	void init() const { if (m_p) m_p->init(); }
	SimpleKey hash() const { return m_p ? m_p->hash() : 0; }
	std::string name() const { return m_p ? m_p->name() : ""; }

	std::shared_ptr<GenericComputeImpl> const& p() const { return m_p; }

protected:
	std::shared_ptr<GenericComputeImpl> m_p;
};

template <class _Info = VoidInfo, class _Element = float>
class ComputeImpl: public GenericComputeImpl
{
public:
	using Info = _Info;
	using Element = _Element;

	virtual Info info() { return Info(); }
	virtual void genericCompute(std::vector<uint8_t>& _v)
	{
		std::vector<Element> v;
		compute(v);
		_v.resize(v.size() * sizeof(_Element));
		memcpy(_v.data(), v.data(), _v.size());
	}
	virtual void compute(std::vector<Element>& _v) { _v.clear(); }
	virtual lb::foreign_vector<Element> get();

protected:
	virtual size_t elementSize() const { return sizeof(_Element); }
	virtual char const* elementTypeName() const { return typeid(_Element).name(); }
};

class Hasher
{
public:
	virtual ~Hasher() {}
	virtual SimpleKey hash(void const* _this) = 0;
};

template <class _T, class ... _P>
class MemberMapMaker: public Hasher
{
public:
	template <class _C> MemberMapMaker(_C const* _this, _T const& _t, _P const& ... _p): m_offset((intptr_t)&_t - (intptr_t)_this), m_others(_this, _p ...) {}
	virtual SimpleKey hash(void const* _this) { return doHash(_this); }
	SimpleKey doHash(void const* _this) { return generateKey(*(_T const*)((intptr_t)_this + m_offset), m_others.hash(_this)); }

private:
	size_t m_offset;
	MemberMapMaker<_P ...> m_others;
};

template <class _T>
class MemberMapMaker<_T>: public Hasher
{
public:
	template <class _C> MemberMapMaker(_C const* _this, _T const& _t): m_offset((intptr_t)&_t - (intptr_t)_this) {}
	virtual SimpleKey hash(void const* _this) { return doHash(_this); }
	SimpleKey doHash(void const* _this) { return generateKey(*(_T const*)((intptr_t)_this + m_offset)); }

private:
	size_t m_offset;
};

template <class _Info = VoidInfo, class _Element = float>
class ComputeImplBase: public ComputeImpl<_Info, _Element>
{
protected:
	template <class ... _P> ComputeImplBase(_P const& ... _p): m_members(new MemberMapMaker<_P ...>(this, _p ...)) {}
	virtual ~ComputeImplBase() { delete m_members; }
	virtual SimpleKey hash() const { return generateKey(m_members->hash(this), this->name()); }

private:
	Hasher* m_members;
};

template <class _Impl> class ComputeBase;

template <class _Element = float, class _Info = VoidInfo>
class Compute: public GenericCompute
{
public:
	using Info = _Info;
	using Element = _Element;

	Compute() {}
	Compute(ComputeImpl<_Info, _Element>* _p): GenericCompute(_p) {}
	Compute(Compute<Element, Info> const& _p): GenericCompute(_p), m_infoConvertor(_p.m_infoConvertor) {}
	template <class _SubInfo> Compute(Compute<Element, _SubInfo> const& _p, _Info = static_cast<_Info const&>(_SubInfo())):
		GenericCompute((GenericCompute const&)_p),
		m_infoConvertor([=](GenericComputeImpl* i) -> Info
		{
			auto orig = _p.m_infoConvertor;
			_SubInfo ret = orig ? orig(i) : dynamic_cast<ComputeImpl<_SubInfo, Element>*>(i)->info();
			return ret;
		})
	{}

	Info info() const { return m_p ? m_infoConvertor ? m_infoConvertor(m_p.get()) : p()->info() : Info(); }
	lb::foreign_vector<Element> get() const { return m_p ? p()->get() : lb::foreign_vector<Element>(); }

	std::shared_ptr<ComputeImpl<_Info, _Element> > p() const { return std::static_pointer_cast<ComputeImpl<_Info, _Element> >(m_p); }

	std::function<Info(GenericComputeImpl*)> m_infoConvertor;
};

template <class _Info = VoidInfo, class _Element = float> SimpleKey generateKey(Compute<_Element, _Info> const& _c) { return _c.hash(); }

class ComputeRegistrar
{
public:
	virtual ~ComputeRegistrar() {}

	static ComputeRegistrar* get() { if (!s_this) s_this = new ComputeRegistrar; return s_this; }
	static Compute<float, PCMInfo> feeder() { return get()->createFeeder(); }

	bool isFirst() const { return m_time == 0; }

	void compute(GenericCompute const& _c) { compute(_c.p().get()); }

	lb::foreign_vector<uint8_t> compute(GenericComputeImpl* _c);
	void init();
	void beginTime(lb::Time _t);
	void endTime(lb::Time _t) { onEndTime(_t); }
	void fini() { onFini(); }
	bool store(GenericCompute const& _p) { return onStore(_p); }

	lb::Time time() const { return m_time; }

protected:
	ComputeRegistrar() {}

	virtual void onInit() {}
	virtual void onFini() {}
	virtual void onBeginTime(lb::Time _newTime) { (void)_newTime; }
	virtual void onEndTime(lb::Time _oldTime) { (void)_oldTime; }
	virtual bool onStore(GenericCompute const&) { return true; }	///< @returns true iff data is already computed and available.
	virtual void insertMemo(SimpleKey _operation) { m_memos.insert(std::make_pair(_operation, std::make_pair(std::vector<uint8_t>(), lb::foreign_vector<uint8_t>()))); }
	virtual Compute<float, PCMInfo> createFeeder() { cwarn << "Creating null feeder!"; return Compute<float, PCMInfo>(nullptr); }

	static thread_local ComputeRegistrar* s_this;

	std::unordered_map<SimpleKey, std::pair<std::vector<uint8_t>, lb::foreign_vector<uint8_t> > > m_memos;
	lb::Time m_time = UndefinedTime;
};

template <class _Info, class _Element>
lb::foreign_vector<_Element> ComputeImpl<_Info, _Element>::get()
{
	return (lb::foreign_vector<_Element>)ComputeRegistrar::get()->compute(this);
}

template <class _Impl>
class ComputeBase: public Compute<typename _Impl::Element, typename _Impl::Info>
{
public:
	template <class ... _P> ComputeBase(_P ... _p): Compute<typename _Impl::Element, typename _Impl::Info>(new _Impl(_p ...)) {}
};

}


