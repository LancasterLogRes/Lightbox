#include <boost/algorithm/string.hpp>
#include <Common/Global.h>
#include "GUIApp.h"
#include "Slate.h"
#include "Global.h"
#include "MarkdownView.h"
using namespace std;
using namespace lb;

MarkdownViewBody::MarkdownViewBody(std::string const& _text, Color _color, Font const& _font):
	m_text(_text),
	m_font(_font),
	m_color(_color)
{
}

MarkdownViewBody::~MarkdownViewBody()
{
}

bool MarkdownViewBody::event(Event* _e)
{
	return Super::event(_e);
}

struct State
{
	Font f;
	Color c;
};

struct InterpreterState
{
	vector<State> stack;
	fCoord cursor;
	unsigned index = 0;
	char thisChar = ' ';
	char nextChar = ' ';
	bool visible = false;
	bool doBreak = false;
	bool doNewLine = false;
};

class RichTextInterpreter
{
public:
	RichTextInterpreter(iRect _r):
		m_bounds(_r)
	{
		m_state.stack = { { GUIApp::style().regular, GUIApp::style().fore } };
		m_state.cursor = (fCoord)m_bounds.pos();
		m_state.index = 0;
	}

	State const& state() const { return m_state.stack.back(); }
	void setState(State const& _s) { m_state.stack = { _s }; }

	void render(string const& _text, Slate const& _slate)
	{
		m_lastNewLineIndex = m_state.index;
		m_lastBreak = m_state;
		m_text = _text;
		m_slate = &_slate;
		for (m_state.index = 0; m_state.index <= m_text.size() && m_state.cursor.y() < m_bounds.bottom();)
		{
			interpret(m_state, false);
		}
	}

private:
	void interpretCommand(string const& _cmd, InterpreterState& _state)
	{
		if (_cmd.size() == 0)
			return;
		if (_cmd == "\\")
		{
			_state.visible = true;
			_state.thisChar = '\\';
			_state.nextChar = ' ';	// TODO: make work properly
		}
		else if (_cmd == "^")
			_state.stack.push_back(_state.stack.back());
		else if (_cmd == ".")
		{
			assert(_state.stack.size() > 1);
			_state.stack.pop_back();
		}
		else if (_cmd[0] == 'b')
		{
			_state.stack.back().f.setBold(_cmd.size() == 1 || !_cmd[1] == '0');
		}
		else if (_cmd[0] == 'n')
		{
			_state.visible = false;
			_state.doBreak = true;
			_state.doNewLine = true;
		}
		else if (_cmd[0] == 'f' && _cmd.size() > 1)
		{
			_state.stack.back().c = Color::interpret(_cmd.substr(1));
		}
		else if (_cmd[0] == 's')
		{
			_state.stack.back().f.setMmSize(atof(_cmd.substr(1).c_str()));
		}
	}

	char nextVisible(unsigned _i) const
	{
		// TODO: won't work properly for commands.
		for (unsigned n = _i + 1; n < m_text.size(); ++n)
			if (m_text[n] == '\\')
			{
				n++;
				if (m_text[n] == '\\')
					return m_text[n];
				else
					return ' ';
			}
			else if (m_text[n] != '\n')
				return m_text[n];
		return ' ';
	}

	void interpret(InterpreterState& _state, bool _renderPass)
	{
		// Reset volatile members.
		_state.doBreak = false;
		_state.doNewLine = false;
		_state.visible = false;
		_state.thisChar = ' ';
		_state.nextChar = ' ';

		char thisChar = _state.index < m_text.size() ? m_text[_state.index] : ' ';
		char nextChar = nextVisible(_state.index);

		if (thisChar == ' ')
		{
			_state.doBreak = true;
			_state.visible = true;
			_state.thisChar = thisChar;
			_state.nextChar = nextChar;
		}
		else if (thisChar == '\n')
		{
			_state.doBreak = true;
			_state.visible = true;
			_state.thisChar = ' ';
			_state.nextChar = ' ';
		}
		else if (thisChar == '\\' && _state.index + 1 < m_text.size()) // command...
		{
			thisChar = m_text[++_state.index];
			if (thisChar == '\'')
			{
				for (unsigned indexFrom = ++_state.index; _state.index < m_text.size(); ++_state.index)
					if (m_text[_state.index] == '\'')
					{
						interpretCommand(m_text.substr(indexFrom, _state.index - indexFrom), _state);
						break;
					}
			}
			else
				interpretCommand(string(&thisChar, 1), _state);
		}
		else
		{
			_state.visible = true;
			_state.thisChar = thisChar;
			_state.nextChar = nextChar;
		}

		if (_state.visible)
		{
			// Character is visible - measure it, adjust line size and cursor accordingly then check to see if we've overrun.
			if (_renderPass)
				m_slate->text(_state.stack.back().f, (iCoord)_state.cursor, string(&thisChar, 1), _state.stack.back().c, AtLeft|AtTop);
//			cnote << "Pos '" << thisChar << nextChar << "'" << "@" << _state.cursor;
			_state.cursor += _state.stack.back().f.pxMetrics(_state.thisChar, _state.nextChar).advance;
//			cnote << "->" << _state.cursor;
			if (!_renderPass && _state.cursor.x() > m_bounds.right())
			{
				// Overrun...
				if (m_lastNewLineIndex == m_lastBreak.index)
				{
					// No word-break on current line - force newline...
					_state.index--;
					_state.doBreak = true;
				}
				// Do a new line
				_state.doNewLine = true;
			}
		}

		if (!_renderPass)
		{
			if (_state.doBreak)
			{
				// At a breakpoint - render word from lastBreak.index up to _state.index.
				for (; m_lastBreak.index <= _state.index;)
					interpret(m_lastBreak, true);
			}

			if (_state.doNewLine)
			{
				// At a new-line
				m_lastBreak.cursor = fCoord(m_bounds.x(), m_lastBreak.cursor.y() + m_lastBreak.stack.back().f.pxMetrics('\n', ' ').advance.h());
				_state = m_lastBreak;
				m_lastNewLineIndex = _state.index;
				_state.index--;	// we'll increment it later and we want to reinterpret from exactly this point onwards.
			}
		}
		_state.index++;
	}

	// Specific to a particular render.
	iRect m_bounds;
	string m_text;
	unsigned m_lastNewLineIndex;
	Slate const* m_slate;

	InterpreterState m_state;
	InterpreterState m_lastBreak;
};

void MarkdownViewBody::draw(Slate const& _c, unsigned)
{
	// Wrap text.
	RichTextInterpreter i(rect());
	State s = i.state();
	if (m_font.isValid())
		s.f = m_font;
	if (m_color.isValid())
		s.c = m_color;
	i.setState(s);
	i.render(m_text, _c);
}
