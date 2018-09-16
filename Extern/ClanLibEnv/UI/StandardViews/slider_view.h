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
**    Mark Page
**    Artem Khomenko
*/

#pragma once

#include "../View/view.h"

namespace clan
{
	class SliderViewImpl;

	class SliderView : public View
	{
	public:
		SliderView();

		std::shared_ptr<View> track() const;
		std::shared_ptr<View> thumb() const;

		bool vertical() const;
		bool horizontal() const;

		void set_vertical();
		void set_horizontal();

		bool disabled() const;
		void set_disabled();
		void set_enabled();

		int max_position() const;
		int position() const;
		int min_position() const;

		void set_max_position(int v);
		void set_position(int v);
		void set_min_position(int v);

		void set_tick_count(int tick_count);
		void set_page_step(int page_step);

		/// When true the position can be only in integer amount of tick_count.
		void set_lock_to_ticks(bool lock);

		std::function<void()> &func_value_changed();

		void layout_children(Canvas &canvas) override;

	private:
		std::shared_ptr<SliderViewImpl> impl;
	};
}
