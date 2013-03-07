#pragma once

#if LIGHTBOX_ANDROID

#include <functional>
#include <pthread.h>
#include "Global.h"

namespace Lightbox
{

class thread
{
public:
	thread(): m_p(0) {}

	template <class _T> thread(_T const& _f): m_p(0)
	{
		Init i = { _f, false };
		pthread_create(&m_p, nullptr, exec, &i);
		while (!i.ok)
			usleep(20);
	}
	~thread()
	{
		assert(!joinable());
	}

	void detach() { m_p = 0; }
	bool joinable() const { return !!m_p; }
	void join() { pthread_join(m_p, nullptr); }

private:
	struct Init { std::function<void()> f; bool ok; };
	static void* exec(void* _p) { std::function<void()> f = ((Init*)_p)->f; ((Init*)_p)->ok = true; f(); return nullptr; }

	pthread_t m_p;
};

class mutex
{
public:
	mutex()
	{
		pthread_mutex_init(&m_p, nullptr);
	}

	~mutex()
	{
		pthread_mutex_destroy(&m_p);
	}

	void lock()
	{
		pthread_mutex_lock(&m_p);
	}

	bool try_lock()
	{
		return !pthread_mutex_trylock(&m_p);
	}

	void unlock()
	{
		pthread_mutex_unlock(&m_p);
	}

private:
	pthread_mutex_t m_p;
};

template <class _T>
class lock_guard
{
public:
	lock_guard(_T& _t): m_t(_t) { m_t.lock(); }
	~lock_guard() { m_t.unlock(); }

private:
	_T& m_t;
};

}

#elif LIGHTBOX_PI

#include <boost/thread.hpp>

namespace Lightbox
{

using boost::thread;
using boost::mutex;

}

#else

#include <thread>
#include <mutex>

namespace Lightbox
{

using std::thread;
using std::mutex;

}

#endif
