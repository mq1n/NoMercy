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
**    Artem Khomenko
*/

#pragma once

#include "../View/view.h"

namespace clan
{
	class ScrollBarViewImpl;

	class ScrollBarView : public View
	{
	public:
		// TODO: decide if "render_button_arrows" is this the best way of controlling rendering
		ScrollBarView(bool render_button_arrows = true);

		std::shared_ptr<View> button_decrement() const;
		std::shared_ptr<View> button_increment() const;
		std::shared_ptr<View> track() const;
		std::shared_ptr<View> thumb() const;
		std::shared_ptr<View> thumb_grip() const;

		bool vertical() const;
		bool horizontal() const;

		void set_vertical();
		void set_horizontal();

		bool disabled() const;
		void set_disabled();
		void set_enabled();

		double line_step() const;
		double page_step() const;

		void set_line_step(double value);
		void set_page_step(double value);

		/// When true the position can be only in integer amount of line_step.
		void set_lock_to_line(bool lock);

		double min_position() const;
		double max_position() const;
		double position() const;

		void set_min_position(double value);
		void set_max_position(double value);
		void set_range(double min_value, double max_value);
		void set_position(double value);

		Signal<void()> &sig_scroll();

		void layout_children(Canvas &canvas) override;

	private:
		std::shared_ptr<ScrollBarViewImpl> impl;
	};
}
