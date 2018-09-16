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

#include "event.h"
#include "../../Core/Math/point.h"

namespace clan
{
	/// Pointer button
	enum class PointerButton
	{
		none,       /// No button specified
		left,       /// Left button
		right,      /// Right button
		middle,     /// Middle/wheel button
		wheel_up,   /// Wheel up tick
		wheel_down, /// Wheel down tick
		xbutton1,   /// Extra button 1
		xbutton2,   /// Extra button 2
		xbutton3,   /// Extra button 3
		xbutton4,   /// Extra button 4
		xbutton5,   /// Extra button 5
		xbutton6    /// Extra button 6
	};

	/// Pointer event type
	enum class PointerEventType
	{
		none,              /// No event type specified
		enter,             /// Pointer entered area
		leave,             /// Pointer left area
		move,              /// Pointer moved above area
		press,             /// Pointer pressed
		release,           /// Pointer released
		double_click,      /// Pointer double click pressed
		promixity_change   /// Pen proximity changed
	};

	/// Pointer event
	class PointerEvent : public EventUI
	{
	public:
		PointerEvent(PointerEventType type, PointerButton button, const Pointf &pos, bool alt_down, bool shift_down, bool ctrl_down, bool cmd_down) :
			_type(type), _button(button), _pos(pos), _alt_down(alt_down), _shift_down(shift_down), _ctrl_down(ctrl_down), _cmd_down(cmd_down)
		{
		}

		/// Pointer event type
		PointerEventType type() const { return _type; }

		/// Pointer button relevant for the event
		PointerButton button() const { return _button; }

		/// Pointer position relative to local view content coordinates
		Pointf pos(View *view) const;
		Pointf pos(const std::shared_ptr<View> &view) const;

		/// Set event pointer position relative to local view content coordinates
		void set_pos(View *view, const Pointf &pos);

		/// True if the alt key was down
		bool alt_down() const { return _alt_down; }

		/// True if the shift key was down
		bool shift_down() const { return _shift_down; }

		/// True if the control key was down
		bool ctrl_down() const { return _ctrl_down; }

		/// True if the command key was down
		bool cmd_down() const { return _cmd_down; }

	private:
		PointerEventType _type = PointerEventType::enter;
		PointerButton _button = PointerButton::none;
		Pointf _pos;
		bool _alt_down = false;
		bool _shift_down = false;
		bool _ctrl_down = false;
		bool _cmd_down = false;
	};
}
