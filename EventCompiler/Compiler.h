#pragma once

#include <Common/FFT.h>
#include <EventCompiler/EventCompiler.h>

namespace lb
{

/** @brief Potentially multi-channel audio stream abstract class.
 * Specialisations could be a file input, a sound card input or perhaps even a procedural generator.
 */
class AudioStream
{
public:
	virtual unsigned rate() const = 0;	// samples per second
	virtual unsigned hop() const = 0;	// samples per iteration
	virtual void iterate() = 0;	// do an iteration
	virtual void copyTo(unsigned _channel, Fixed<11, 21>* _p) = 0;		// copy samples of given channel to given buffer (as I16F16 format).
	virtual void copyTo(unsigned _channel, Fixed<16, 16>* _p) = 0;		// copy samples of given channel to given buffer (as I16F16 format).
	virtual void copyTo(unsigned _channel, Fixed<1, 15>* _p) = 0;		// copy samples of given channel to given buffer (as I1F15 format).
	virtual void copyTo(unsigned _channel, float* _p) = 0;			// copy samples of given channel to given buffer (as I1F15 format).

	Time hopTime() const { return toBase(hop(), rate()); }

protected:
	template <class _F, class _T> void translateTo(unsigned _channel, _T* _p)
	{
		_F buf[hop()];
		copyTo(_channel, buf);
		for (auto b = &(buf[0]), be = &(buf[hop()]); b != be; ++b, ++_p)
			*_p = (float)*b;
	}
};

/** @brief Class for processing an input audio stream and feeding an event compiler.
 */
template <class Scalar>
class Compiler
{
public:
	typedef typename FFT<Scalar>::InputType InputType;
	typedef typename FFT<Scalar>::OutputType OutputType;

	Compiler(std::string const& _ecName = std::string(), unsigned _inputChannel = 0, AudioStream* _input = nullptr):
		m_input(_input),
		m_inputChannel(_inputChannel),
		m_ecName(_ecName)
	{}

	Compiler(EventCompiler const& _ec, unsigned _inputChannel = 0, AudioStream* _input = nullptr):
		m_input(_input),
		m_inputChannel(_inputChannel),
		m_ec(_ec)
	{}

	template <class _F> void init(AudioStream* _input, _F const& _ecMaker)
	{
		m_input = _input;
		if (m_ec.isNull())
			if (m_ecName.empty())
				m_ec = NullEventCompiler<Scalar>();
			else
				m_ec = _ecMaker(m_ecName);

		init();
	}

	void init()
	{
		assert(m_input);
		m_hop = fromBase(m_input->hop(), m_input->rate());

		m_in.clear();
		m_in.resize(m_hop, 0);

		m_ec.init(toBase(m_input->hop(), m_input->rate()), toBase(2, m_input->rate()));
	}

	void fini()
	{
		m_ses.clear();
		m_in.clear();
	}

	void setInput(AudioStream* _input) { m_input = _input; }
	void setInputChannel(unsigned _inputChannel) { m_inputChannel = _inputChannel; }
	void setEventCompilerName(char const* _ecName) { m_ecName = _ecName; m_ec = EventCompiler(); }	///< Overrides previous setEventCompiler
	void setEventCompiler(EventCompiler const& _ec) { m_ec = _ec; }	///< Overrides previous setEventCompilerName

	void iterate() { iterateAudio(); iterateEvents(); }
	void iterateAudio()
	{
		m_input->copyTo(m_inputChannel, m_in.data());

		// Scale
		OutputType mi = 0;
		auto ie = m_in.data() + m_input->hop();
		for (auto i = m_in.data(); i != ie; ++i)
			mi = std::max(mi, abs(*i));

		OutputType g = std::min(m_gain, 1.f / (float)mi);
		if (g != (OutputType)1)
			for (auto i = m_in.data(); i != ie; ++i)
				*i *= g;
		m_vu = maxInRange(m_in);
		m_highVu = std::max(m_vu, m_highVu);
	}
	void iterateEvents() { m_ses = m_ec.compile(m_in); }

	float gain() const { return m_gain; }
	void setGain(float _g) { m_gain = _g; }
	float vu() const { return m_vu; }
	float highVu() const { float ret = m_highVu; m_highVu = -1.f; return ret; }
	std::string name() const { return m_ec.name(); }

	StreamEvents const& events() const { return m_ses; }

private:
	AudioStream* m_input = nullptr;
	unsigned m_inputChannel = 0;

	Time m_hop;

	std::vector<OutputType> m_in;

	std::string m_ecName;
	EventCompiler m_ec;

	StreamEvents m_ses;

	float m_vu = 0.f;
	mutable float m_highVu = 0.f;
	float m_gain = 1.f;
};

}


