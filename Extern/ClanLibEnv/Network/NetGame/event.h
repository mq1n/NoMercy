/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "event_value.h"

namespace clan
{
	/// \addtogroup clanNetwork_NetGame clanNetwork NetGame
	/// \{

	/// \brief NetGameEvent
	class NetGameEvent
	{
	public:
		/// Constructs a NetGameEvent
		NetGameEvent(const std::string &name, std::vector<NetGameEventValue> arg = {});

		/// \return The name of this event.
		std::string get_name() const { return name; };

		/// \return The number of arguments stored in this event.
		unsigned int get_argument_count() const;

		/// Retrieves an argument in this event.
		/// \param index Index number of the argument to retrieve.
		/// \return A NetGameEventValue object containing the argument value.
		NetGameEventValue get_argument(unsigned int index) const;

		/// Adds an argument into this event.
		/// \param value The argument to store inside this event.
		void add_argument(const NetGameEventValue &value);

		/// \return A string representation of this event, including all of its arguments.
		std::string to_string() const;

	private:
		std::string name;
		std::vector<NetGameEventValue> arguments;
	};

	/// \}
}

