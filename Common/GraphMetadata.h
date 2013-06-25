#pragma once

#include <string>
#include <Common/Trivial.h>
#include <Common/SimpleKey.h>

namespace lb
{

enum GraphHint { Acyclic };
LIGHTBOX_FLAGS_TYPE(GraphHint, GraphHints);

/**
 * @brief Data concerning, but not expressing, a graph.
 * url is an identifier for this graph within the context of an analysis; it is valid
 * between subsequent executions. It is stored within this class as a convenience; this class
 * does not administer the value; that is left to GraphMan, which happens whenever an instance
 * of this class is registered with GraphMan.
 *
 * For event compiler graphs, it will be composed of some reference to the event compiler's
 * instance (e.g. its name) together with the operation at hand.
 *
 * key is the operation key for this graph's data. Multiple GraphMetadata instances may have
 * the same key, meaning that they refer to the same underlying graph data. For event
 * compiler graphs it is a hash of the event compiler's class name, its version and any
 * parameters it has. System graphs may have reserved keys (e.g. wave data, 0), or may hash
 * dependent parameters.
 *
 * Note: The first axis is always the Y-axis when plotting against time (i.e. for line charts
 * it's the point value and for spectra it's the bin value). The axes increment through X & Y
 * (and Z, in the future, perhaps) ending with the "value" axis, which is the Y-axis when plotting
 * a point in time.
 * ValueAxis can be passed to axis() to retrive the last axis.
 */
class GraphMetadata
{
public:
	enum { ValueAxis = -1, XAxis = 0, YAxis = 1 };

	struct Axis
	{
		std::string label;
		lb::XOf transform;
		lb::Range range;
	};

	typedef std::vector<Axis> Axes;

	GraphMetadata() {}
	GraphMetadata(Axes const& _axes, std::string const& _title = "", bool _rawSource = false): m_rawSource(_rawSource), m_operationKey(0), m_title(_title), m_axes(_axes) {}
	GraphMetadata(std::string const& _title, bool _rawSource = false): m_rawSource(_rawSource), m_operationKey(0), m_title(_title) {}
	GraphMetadata(lb::SimpleKey _operationKey, Axes const& _axes, std::string const& _title = "", bool _rawSource = false): m_rawSource(_rawSource), m_operationKey(_operationKey), m_title(_title), m_axes(_axes) {}
	GraphMetadata(lb::SimpleKey _operationKey, std::string const& _title = "", bool _rawSource = false): m_rawSource(_rawSource), m_operationKey(_operationKey), m_title(_title) {}

	bool isNull() const { return m_title.empty() || m_axes.empty(); }
	bool isValid() const { return !isNull(); }
	bool isRegistered() const { return isValid() && !m_url.empty(); }

	explicit operator bool() const { return isValid(); }

	bool isRawSource() const { return m_rawSource; }
	lb::SimpleKey operationKey() const { return m_operationKey; }
	std::string const& url() const { return m_url; }
	std::string const& title() const { return m_title; }
	GraphHints hints() const { return m_hints; }

	void setHints(GraphHints _hints) { m_hints = _hints; }
	void setTitle(std::string const& _title) { m_title = _title; }
	void setRawSource(bool _rawSource = true) { m_rawSource = _rawSource; }
	void setOperationKey(lb::SimpleKey _k) { m_operationKey = _k; }

	Axis const& axis(unsigned _i = ValueAxis) const { return m_axes[lb::defaultTo((int)_i, (int)m_axes.size() - 1, (int)ValueAxis)]; }
	Axes const& axes() const { return m_axes; }
	void setAxes(Axes const& _as) { m_axes = _as; }

	void setUrl(std::string const& _url) { m_url = _url; }

protected:
	bool m_rawSource = false;
	lb::SimpleKey m_operationKey = (unsigned)-1;
	std::string m_title;

	Axes m_axes = { { "", lb::XOf(), lb::Range(0,1) } };

	GraphHints m_hints;

	std::string m_url;
};

static const GraphMetadata NullGraphMetadata;

}
