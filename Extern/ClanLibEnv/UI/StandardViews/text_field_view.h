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

#include "../View/view.h"
#include "label_view.h"

namespace clan
{
	class Font;
	class TextFieldViewImpl;

	class TextFieldView : public View
	{
	public:
		TextFieldView();
		~TextFieldView();

		int preferred_size() const;
		void set_preferred_size(int num_characters);

		std::string text() const;
		void set_text(const std::string &value);

		std::string placeholder() const;
		void set_placeholder(const std::string &value);

		TextAlignment text_alignment() const;
		void set_text_alignment(TextAlignment value);

		bool is_read_only() const;
		void set_read_only(bool value = true);

		bool is_lowercase() const;
		void set_lowercase(bool value = true);

		bool is_uppercase() const;
		void set_uppercase(bool value = true);

		bool is_password_mode() const;
		void set_password_mode(bool value = true);

		int max_length() const;
		void set_max_length(int length);

		std::string selection() const;
		size_t selection_start() const;
		size_t selection_length() const;
		void set_selection(size_t pos, size_t length);
		void clear_selection();
		void delete_selected_text();
		void select_all();
		void set_select_all_on_focus_gain(bool value);

		int cursor_pos() const;
		void set_cursor_pos(int pos);
		void set_cursor_drawing_enabled(bool value);

		int text_int() const;
		void set_text(int number);

		float text_float() const;
		void set_text(float number, int num_decimal_places = 6);

		void set_numeric_mode(bool enable = true, bool decimals = false);
		void set_input_mask(const std::string &mask);
		void set_decimal_character(const std::string &decimal_char);

		Signal<void(KeyEvent &)> &sig_before_edit_changed();
		Signal<void(KeyEvent &)> &sig_after_edit_changed();
		Signal<void()> &sig_selection_changed();
		Signal<void()> &sig_enter_pressed();

		/// Update yourself with draw_without_layout() and with delay to prevent too frequent calls.
		void needs_render_delayed();

	protected:

		void render_content(Canvas &canvas) override;
		float calculate_preferred_width(Canvas &canvas) override;
		float calculate_preferred_height(Canvas &canvas, float width) override;
		float calculate_first_baseline_offset(Canvas &canvas, float width) override;
		float calculate_last_baseline_offset(Canvas &canvas, float width) override;

	private:
		std::unique_ptr<TextFieldViewImpl> impl;
	};
}
