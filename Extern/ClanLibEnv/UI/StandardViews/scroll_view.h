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
	enum class ContentOverflow
	{
		hidden,
		scroll,
		automatic
	};
	
	class ScrollBarView;
	class ScrollViewImpl;
	
	class ScrollView : public View
	{
	public:
		ScrollView();
		~ScrollView();
		
		std::shared_ptr<ScrollBarView> scrollbar_x_view() const;
		std::shared_ptr<ScrollBarView> scrollbar_y_view() const;
		
		std::shared_ptr<View> content_view() const;
		
		ContentOverflow overflow_x() const;
		ContentOverflow overflow_y() const;
		void set_overflow_x(ContentOverflow value);
		void set_overflow_y(ContentOverflow value);
		void set_overflow(ContentOverflow value_x, ContentOverflow value_y);
		
		Pointf content_offset() const;
		void set_content_offset(const Pointf &offset, bool animated = false);
		
		void layout_children(Canvas &canvas) override;

		// Enable or disable.
		bool disabled() const;
		void set_disabled();
		void set_enabled();

		/// Process mouse wheel events. When Shift pressed, scrolls horizontally, otherwise vertically.
		void on_pointer_press(PointerEvent &e);

	protected:
		float calculate_preferred_width(Canvas &canvas) override;
		float calculate_preferred_height(Canvas &canvas, float width) override;
		float calculate_first_baseline_offset(Canvas &canvas, float width) override;
		float calculate_last_baseline_offset(Canvas &canvas, float width) override;
		
	private:
		std::unique_ptr<ScrollViewImpl> impl;
	};
}
