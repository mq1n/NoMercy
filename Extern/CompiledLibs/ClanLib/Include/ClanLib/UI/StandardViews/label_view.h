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
**    Artem Khomenko (modified set_text())
*/

#pragma once

#include "../View/view.h"
#include "../../Display/Font/font.h"
#include "../../Display/2D/color.h"

namespace clan
{
	enum class TextAlignment
	{
		left,
		right,
		center,
		justify/*,
		start,
		end*/
	};

	enum class LineBreakMode
	{
		//word_wrapping,
		//char_wrapping,
		clipping,
		truncating_head,
		truncating_tail,
		truncating_middle
	};

	class LabelViewImpl;

	class LabelView : public View
	{
	public:
		LabelView();

		std::string text() const;
		
		/// \brief Set text for display to the user
		///
		/// \param value = Text
		/// \param force_no_layout = when false or omitted, after setting the text occurs recalculate sizes 
		/// and positions of elements, what means complete repaint the entire window. When true - simply drawing
		/// the new text. Also make sure that the style property "background color" is set for this or its parents.
		void set_text(const std::string &value, bool force_no_layout = false);

		TextAlignment text_alignment() const;
		void set_text_alignment(TextAlignment alignment);

		LineBreakMode line_break_mode() const;
		void set_line_break_mode(LineBreakMode value);

		void layout_children(Canvas &canvas) override;

		/// Update the font in according to style.
		void reset_font();

	protected:
		void render_content(Canvas &canvas) override;
		float calculate_preferred_width(Canvas &canvas) override;
		float calculate_preferred_height(Canvas &canvas, float width) override;
		float calculate_first_baseline_offset(Canvas &canvas, float width) override;
		float calculate_last_baseline_offset(Canvas &canvas, float width) override;

	private:
		std::shared_ptr<LabelViewImpl> impl;
	};

}
