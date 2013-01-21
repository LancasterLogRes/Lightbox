#pragma once

#if LIGHTBOX_ANDROID

#include <functional>
#include <pthread.h>

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

}

#else

#include <thread>

namespace Lightbox
{

using std::thread;

}

#endif
