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

#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include <Common/Global.h>
#include "EventCompilerImpl.h"

namespace lb
{
struct EventCompilerFactoryDefinition
{
	std::function<EventCompilerImpl*()> factory;
	unsigned version;
};
typedef std::unordered_map<std::string, EventCompilerFactoryDefinition> EventCompilerFactories;
}

#define LIGHTBOX_EVENTCOMPILER_LIBRARY_HEADER \
	extern "C" __attribute__ ((visibility ("default"))) lb::EventCompilerFactories const& eventCompilerFactories();

#define LIGHTBOX_EVENTCOMPILER_LIBRARY \
	LIGHTBOX_FINALIZING_LIBRARY \
	extern "C" __attribute__ ((visibility ("default"))) lb::EventCompilerFactories const& eventCompilerFactories() { static lb::EventCompilerFactories s_ret; return s_ret; } \
	LIGHTBOX_EVENTCOMPILER_LIBRARY_HEADER

#define LIGHTBOX_EVENTCOMPILER(O, V) \
	auto g_reg ## O = (const_cast<lb::EventCompilerFactories&>(eventCompilerFactories())[#O] = {[](){return new O;}, V})

