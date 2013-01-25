#pragma once

#include <tuple>
#include <App/App.h>
#include <Common/Color.h>
#include "FontManager.h"
#include "Joint.h"
#include "Frame.h"

namespace Lightbox
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
	fSize thumbSize;
	float thumbOutline;
	Color outlineColor;
};

class GUIApp: public App
{
public:
	GUIApp();
	virtual ~GUIApp();

	static GUIApp* get();
	static Joint& joint() { return get()->m_joint; }
	static Frame const& root() { return get()->m_root; }
	static Style const& style() { return get()->m_style; }
	static FontManager& fontManager() { return get()->m_fontManager; }

	virtual void initGraphics(Display& _d);
	virtual bool drawGraphics();
	virtual void finiGraphics(Display& _d);
	virtual bool motionEvent(int _id, iCoord _pos, int _direction);

	bool lockPointer(int _id, View const& _v);
	bool releasePointer(int _id, View const& _v);
	bool pointerLocked(int _id, View const& _v) const;

private:
	Frame m_root;
	Joint m_joint;
	Style m_style;
	FontManager m_fontManager;

	typedef std::pair<ViewBody*, unsigned> ViewLayer;

	struct CachePos
	{
		uRect pos;
		unsigned index;
	};

	struct ImageCache
	{
		explicit ImageCache(uSize _ps);
		bool fit(iRect _g, ViewLayer _v);

		Framebuffer fb;
		Texture2D tx;
		Buffer<float> geom;
		std::vector<float> collated;
		unsigned nextfree;
		std::multimap<unsigned, std::pair<unsigned, unsigned> > rows; // rowheight -> (ypos, width)
		std::map<ViewLayer, CachePos> vs;
	};
	std::vector<ImageCache> m_cache;

	std::array<View, s_maxPointers> m_pointerLock;
};

}


