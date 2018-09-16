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

#include <memory>

namespace clan
{
	class View;

	/// UI event dispatch phase
	enum class EventUIPhase
	{
		none,      /// Event is not currently in any dispatch phase
		capturing, /// Capture phase (inverse bubble from root to the target view)
		at_target, /// Currently being dispatched to the target view
		bubbling   /// Event bubbling up from target view to root
	};

	/// Base class for events being dispatched through the view hiarchy
	class EventUI
	{
	public:
		virtual ~EventUI() { }

		/// Current active event phase during dispatch
		EventUIPhase phase() const { return _phase; }

		/// The target view the event is fired for
		std::shared_ptr<View> target() { return _target; }

		/// View the event is currently being dispatched to
		std::shared_ptr<View> current_target() { return _current_target; }

		/// Flag if the event default action should be executed after dispatch
		bool default_prevented() const { return _default_prevented; }

		/// Prevent default action from being executed after dispatch
		void prevent_default() { _default_prevented = true; }

		//bool immediate_propagation_stopped() const { return _immediate_propagation_stopped; }
		//void stop_immediate_propagation() { _propagation_stopped = true; _immediate_propagation_stopped = true; }

		/// Flag if event propagation should stop
		bool propagation_stopped() const { return _propagation_stopped; }

		/// Stops event from propagating further
		void stop_propagation() { _propagation_stopped = true; }

		/// Timestamp for event in milliseconds since 1970
		long long timestamp() const { return _timestamp; }

		/// Set event timestamp
		void set_timestamp(long long ts) { _timestamp = ts; }

	private:
		bool _default_prevented = false;
		bool _propagation_stopped = false;
		//bool _immediate_propagation_stopped = true;
		EventUIPhase _phase = EventUIPhase::none;
		std::shared_ptr<View> _target;
		std::shared_ptr<View> _current_target;
		long long _timestamp = 0;

		friend class View;
		friend class ViewImpl;
		friend class ViewTree;
	};
}
