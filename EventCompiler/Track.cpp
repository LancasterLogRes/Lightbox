/* BEGIN COPYRIGHT
 *
 * This file is part of Noted.
 *
 * Copyright ©2011, 2012, Lancaster Logic Response Limited.
 *
 * Noted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Noted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Noted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include "Track.h"
using namespace std;
using namespace Lightbox;

void Track::writeFile(string const& _filename, OutputFormat _f)
{
	ofstream out;
	out.open(_filename);
	if (out)
		write(out, _f);
}

void Track::readFile(string const& _filename, OutputFormat _f)
{
	ifstream in;
	in.open(_filename);
	if (in)
	{
		if (_f == AutoFormat)
		{
			_f = BinaryFormat;
			if (boost::algorithm::to_lower_copy(_filename.substr(_filename.size() - 3)) == "xml")
				_f = XMLFormat;
			else
			{
				char s[5];
				in.readsome(s, 5);
				if (string(s) == "<?xml")
					_f = XMLFormat;
				in.seekg(0, ios::beg);
			}
		}
		read(in, _f);
	}
}

void Track::write(ostream& _out, OutputFormat _f)
{
	if (_f == XMLFormat)
		writeXML(_out);
	else if (_f == BinaryFormat)
		streamOut([&](void const* d, size_t s){ _out.write((char const*)d, s); });
}

void Track::read(istream& _in, OutputFormat _f)
{
	syncPoints.clear();
	syncPoints.push_back(Time(0));
	events.clear();

	if (_f == XMLFormat)
		readXML(_in);
	else if (_f == BinaryFormat)
		streamIn([&](void* d, size_t s){ _in.read((char*)d, s); });
}

void Track::writeXML(ostream& _out)
{
	using boost::property_tree::ptree;
	ptree pt;
	Time last = UndefinedTime;
	ptree* lt = nullptr;
	for (auto const& e: events)
	{
		if (last != e.first)
		{
			assert(last < e.first);
			last = e.first;
			lt = &pt.add("events.time", "");
			lt->put("<xmlattr>.value", last);
			lt->put("<xmlattr>.ms", toMsecs(last));
		}
		StreamEvent const& se = e.second;
		ptree& pte = lt->add(boost::algorithm::to_lower_copy(toString(se.type)), "");
		pte.put("<xmlattr>.strength", se.strength);
		pte.put("<xmlattr>.temperature", se.temperature);
		pte.put("<xmlattr>.position", se.position);
		pte.put("<xmlattr>.channel", se.channel);
		pte.put("<xmlattr>.surprise", se.surprise);
		pte.put("<xmlattr>.constancy", se.constancy);
		pte.put("<xmlattr>.jitter", se.jitter);
		pte.put("<xmlattr>.character", toString(se.character));
	}

	write_xml(_out, pt);
}

void Track::readXML(istream& _in)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_xml(_in, pt);

	foreach (ptree::value_type const& v, pt.get_child("events"))
		if (v.first == "time")
		{
			int64_t ms = v.second.get<int64_t>("<xmlattr>.ms", 0);
			Time t = v.second.get<Time>("<xmlattr>.value", fromMsecs(ms));
			foreach (ptree::value_type const& w, v.second)
				if (w.first != "<xmlattr>")
				{
					StreamEvent se;
					se.type = toEventType(w.first, false);
					se.strength = w.second.get<float>("<xmlattr>.strength", 1.f);
					se.temperature = w.second.get<float>("<xmlattr>.temperature", 0.f);
					se.jitter = w.second.get<float>("<xmlattr>.jitter", 0.f);
					se.constancy = w.second.get<float>("<xmlattr>.constancy", 0.f);
					se.position = w.second.get<int>("<xmlattr>.position", -1);
					se.surprise = w.second.get<float>("<xmlattr>.surprise", 1.f);
					se.character = toCharacter(w.second.get<string>("<xmlattr>.character", "Dull"));
					se.assign(w.second.get<int>("<xmlattr>.channel", CompatibilityChannel));
					events.insert(make_pair(t, se));
					if (se.type == SyncPoint)
						syncPoints.push_back(t);
				}
		}
}

void Track::updateSyncPoints()
{
	syncPoints.clear();
	syncPoints.push_back(Time(0));
	for (auto i: events)
		if (i.second.type == SyncPoint)
			syncPoints.push_back(i.first);
}
