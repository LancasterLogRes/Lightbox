#pragma once

#include <Common/FFT.h>
#include <EventCompiler/EventCompiler.h>

namespace Lightbox
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

	Compiler(unsigned _windowSize = 512, WindowFunction _windowFunction = GaussianWindow, double _windowFunctionParam = 0.5, bool _zeroPhase = true, std::string const& _ecName = std::string(), unsigned _inputChannel = 0, AudioStream* _input = nullptr):
		m_input(_input),
		m_inputChannel(_inputChannel),
		m_windowSize(_windowSize),
		m_windowFunction(_windowFunction),
		m_windowFunctionParam(_windowFunctionParam),
		m_zeroPhase(_zeroPhase),
		m_ecName(_ecName)
	{}

	Compiler(unsigned _windowSize, WindowFunction _windowFunction, double _windowFunctionParam, bool _zeroPhase, EventCompiler const& _ec, unsigned _inputChannel = 0, AudioStream* _input = nullptr):
		m_input(_input),
		m_inputChannel(_inputChannel),
		m_windowSize(_windowSize),
		m_windowFunction(_windowFunction),
		m_windowFunctionParam(_windowFunctionParam),
		m_zeroPhase(_zeroPhase),
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
		m_window = vector_cast<OutputType>(windowFunction(m_windowSize, m_windowFunction, m_windowFunctionParam));

		m_fft.setArity(m_windowSize);
		m_acc.clear();
		m_acc.resize(m_windowSize, 0);

		m_ec.init(m_fft.bands(), toBase(m_input->hop(), m_input->rate()), toBase(2, m_input->rate()));

		cnote << m_window;
	}

	void fini()
	{
		m_ses.clear();
		m_acc.clear();
	}

	void setInput(AudioStream* _input) { m_input = _input; }
	void setInputChannel(unsigned _inputChannel) { m_inputChannel = _inputChannel; }
	void setEventCompilerName(char const* _ecName) { m_ecName = _ecName; m_ec = EventCompiler(); }	///< Overrides previous setEventCompiler
	void setEventCompiler(EventCompiler const& _ec) { m_ec = _ec; }	///< Overrides previous setEventCompilerName
	void setWindowSize(unsigned _windowSize) { m_windowSize = _windowSize; }
	void setWindowFunction(WindowFunction _windowFunction) { m_windowFunction = _windowFunction; }
	void setWindowFunctionParam(double _p) { m_windowFunctionParam = _p; }
	void setZeroPhase(bool _enable) { m_zeroPhase = _enable; }

	void iterate() { iterateAudio(); iterateFFT(); iterateEvents(); }
	void iterateAudio()
	{
		// Shift...
		static const unsigned constPart = m_windowSize - m_input->hop();
		if (constPart < m_windowSize)
			valmove(m_acc.data(), m_acc.data() + m_input->hop(), constPart);

		// Accumulate (misusing m_fft.in as a temporary input buffer)...
		m_input->copyTo(m_inputChannel, m_fft.in());

		float m = 0;
		for (auto f = m_fft.in(), fe = m_fft.in() + m_input->hop(); f != fe; ++f)
			m += (float)*f;
//		cnote << "Incoming:" << (m / m_input->hop());

		// Scale
		InputType mi = 0;
		auto ie = m_fft.in() + m_input->hop();
		for (auto i = m_fft.in(); i != ie; ++i)
			mi = std::max(mi, abs(*i));

		// InputType, if it's Fixed<1, 15> can't go over 1, so we'll have to switch to OutputType for the multiplication.
		OutputType g = std::min(m_gain, 1.f / (float)mi);
		m_vu = (float)mi * (float)g;
		m_highVu = std::max(m_vu, m_highVu);
//		cnote << "Max:" << mi << "(float)Max" << (float)mi << "Max gain:" << (1.f / (float)mi) << "Gain: " << m_gain << "float-Scale:" << std::min(m_gain, 1.f / (float)mi) << "Scale:" << g << "VU" << m_vu;
		auto o = m_acc.data() + constPart;
		for (auto i = m_fft.in(); i != ie; ++i, ++o)
			*o = g * OutputType(*i);
//		cnote << "Buffer: " << mean(m_acc);

		// Prepare waveform for FFT...
		unsigned accOffset = m_zeroPhase ? m_windowSize / 2 : 0;
		// First half (up to accOffset).
		auto w = m_window.data() + accOffset;
		auto a = m_acc.data() + accOffset;
		auto d = m_fft.in();
		for (auto de = m_fft.in() + accOffset; d != de; ++d, ++w, ++a)
			*d = *w * *a;
		// Second half (after accOffset).
		w = m_window.data();
		a = m_acc.data();
		for (auto de = m_fft.in() + m_windowSize; d != de; ++d, ++w, ++a)
			*d = *w * *a;

		m = 0;
		for (auto f = m_fft.in(), fe = m_fft.in() + m_windowSize; f != fe; ++f)
			m += (float)*f;
//		cnote << "FFT-in: " << (m / m_windowSize);
	}
	void iterateFFT() { m_fft.process(); }
	void iterateEvents() { m_ses = m_ec.compile(m_fft.mag(), m_fft.phase(), m_acc); }

	float gain() const { return m_gain; }
	void setGain(float _g) { m_gain = _g; }
	float vu() const { return m_vu; }
	float highVu() const { float ret = m_highVu; m_highVu = -1.f; return ret; }
	std::string name() const { return m_ec.name(); }

	StreamEvents const& events() const { return m_ses; }

private:
	AudioStream* m_input = nullptr;
	unsigned m_inputChannel = 0;

	unsigned m_windowSize = 512;	// ...in samples.
	Time m_hop;
	WindowFunction m_windowFunction = GaussianWindow;
	double m_windowFunctionParam = 0.5;
	bool m_zeroPhase;

	std::vector<OutputType> m_acc;
	std::vector<OutputType> m_window;
	FFT<Scalar> m_fft;

	std::string m_ecName;
	EventCompiler m_ec;

	StreamEvents m_ses;

	float m_vu = 0.f;
	mutable float m_highVu = 0.f;
	float m_gain = 1.f;
};

}


