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
**    Chu Chin Kuan
*/

#pragma once

#include "event.h"
#include <map>

namespace clan
{
	template<class... Params>
	class NetGameEventDispatcher
	{
	public:
		typedef std::function< void(const NetGameEvent &, Params...) > CallbackClass;

		CallbackClass &func_event(const std::string &name) { return event_handlers[name]; }

		/** \brief Dispatches the event object.
		 *  \return true if the event handler is invoked and false if the
		 *          event handler is not found.
		 */
		bool dispatch(const NetGameEvent &game_event, Params... params)
		{
			auto it = event_handlers.find(game_event.get_name());
			if (it != event_handlers.end() && (bool)it->second)
			{
				it->second(game_event, params...);
				return true;
			}
			else
			{
				return false;
			}
		}

	private:
		std::map<std::string, CallbackClass> event_handlers;

	};
}
