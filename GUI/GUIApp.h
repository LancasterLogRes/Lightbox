#pragma once

#include <Common/thread.h>
#include <array>
#include <set>
#include <tuple>
#include <App/App.h>
#include <Common/Time.h>
#include <Common/Color.h>
#include "FontManager.h"
#include "Joint.h"
#include "Frame.h"

namespace lb
{

struct Style
{
	void generateColors(Color _fore);
	Color back;		// usually similar hue to fore, but dark and desaturated.
	Color fore;		// usually fairly bright and vibrant.
	Color high;		// usually complimentary to fore.
	Font big;
	Font bigBold;
	Font regular;
	Font bold;
	Font small;
	Font smallBold;
	Font xsmall;
	Font xsmallBold;
	fSize thumbDiameter;
	fSize lightEdgeSize;
	fSize lightBedSize;
	Color outlineColor;
};

class GUIApp: public App
{
public:
	typedef App Super;
	GUIApp();
	virtual ~GUIApp();

	static GUIApp* get();
	static Joint& joint() { return get()->m_joint; }
	static Frame const& root() { return get()->m_root; }
	static Style const& style() { return get()->m_style; }
	static FontManager& fontManager() { return get()->m_fontManager; }
	static Time runningTime() { return wallTime() - get()->m_startTime; }

	virtual void initGraphics(Display& _d);
	virtual bool drawGraphics();
	virtual void finiGraphics(Display& _d);
	virtual bool motionEvent(int _id, iCoord _pos, int _direction);
	virtual bool keyEvent(int _code, int _direction);
	virtual void go();
	virtual void iterate(Time _d);

	bool lockPointer(int _id, View const& _v);
	bool releasePointer(int _id, View const& _v);
	bool pointerLocked(int _id, View const& _v) const;

	void registerAlive(View const& v);
	void unregisterAlive(View const& v);

private:
	Frame m_root;
	Joint m_joint;
	Style m_style;
	FontManager m_fontManager;
	Time m_startTime;

	LIGHTBOX_STRUCT(2, CachePos, iRect, pos, unsigned, index);

	struct ImageCache
	{
		explicit ImageCache(iSize _ps);
		bool fit(iRect _g, ViewLayerPtr _v);

		Framebuffer fb;
		Texture2D tx;
		Buffer<float> geom;
		std::vector<float> collated;
		unsigned nextfree;
		std::multimap<unsigned, std::pair<unsigned, unsigned> > rows; // rowheight -> (ypos, width)
		std::map<ViewLayerPtr, CachePos> vs;
	};
	std::vector<ImageCache> m_cache;
	int m_showCachePage;

	template <class _S> friend _S& operator<<(_S& _out, ImageCache const& _ic);

	std::array<View, s_maxPointers> m_pointerLock;
	std::set<View> m_alive;
	mutex x_alive;
};

template <class _S> _S& operator<<(_S& _out, GUIApp::ImageCache const& _ic)
{
	_out << _ic.vs;
	return _out;
}

}


