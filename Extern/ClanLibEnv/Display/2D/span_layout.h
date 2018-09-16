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
#include "../../Core/Math/rect.h"
#include "../../Core/Math/size.h"
#include "color.h"

namespace clan
{
	/// \addtogroup clanDisplay_2D clanDisplay 2D
	/// \{

	class Font;
	class GraphicContext;
	class Point;
	class Size;
	class SpanLayout_Impl;
	class Image;
	class SpanComponent;
	class Canvas;

	/// \brief Span Align
	enum SpanAlign
	{
		span_left,
		span_right,
		span_center,
		span_justify
	};

	/// \brief Span layout class
	class SpanLayout
	{
	public:
		SpanLayout();
		~SpanLayout();

		struct HitTestResult
		{
			HitTestResult() : object_id(-1), offset(0) {}

			enum Type
			{
				no_objects_available,
				outside_top,
				outside_left,
				outside_right,
				outside_bottom,
				inside
			} type;

			int object_id;
			int offset;
		};

		/// \brief Clear
		void clear();

		/// \brief Add text
		///
		/// \param text = String
		/// \param font = Font
		/// \param color = Colorf
		/// \param id = value
		void add_text(const std::string &text, const Font &font, const Colorf &color = StandardColorf::white(), int id = -1);

		/// \brief Add image
		///
		/// \param image = Image
		/// \param baseline_offset = value
		/// \param id = value
		void add_image(const Image &image, int baseline_offset = 0, int id = -1);

		template<typename T>

		/// \brief Add component
		///
		/// \param component = T
		/// \param baseline_offset = value
		/// \param id = value
		void add_component(T *component, int baseline_offset = 0, int id = -1);

		/// \brief Layout
		///
		/// \param canvas = Canvas
		/// \param max_width = value
		void layout(Canvas &canvas, int max_width);

		/// \brief Set position
		///
		/// \param pos = Point
		void set_position(const Point &pos);

		/// \brief Get Size
		///
		/// \return size
		Size get_size() const;

		/// \brief Get Rect
		///
		/// \return rect
		Rect get_rect() const;

		/// \brief Get Rect By Id
		///
		/// \return rect
		std::vector<Rect> get_rect_by_id(int id) const;

		/// \brief Hit test
		///
		/// \param canvas = Canvas
		/// \param pos = Point
		///
		/// \return Hit Test Result
		HitTestResult hit_test(Canvas &canvas, const Point &pos);

		/// \brief Draw layout
		///
		/// \param canvas = Canvas
		void draw_layout(Canvas &canvas);

		/// \brief Draw layout generating ellipsis for clipped text
		///
		/// \param canvas = Canvas
		/// \param content_rect = Clipping rectangle
		void draw_layout_ellipsis(Canvas &canvas, const Rect &content_rect);

		/// \brief Set component geometry
		void set_component_geometry();

		/// \brief Find preferred size
		///
		/// \param canvas = Canvas
		///
		/// \return Size
		Size find_preferred_size(Canvas &canvas);

		/// \brief Set selection range
		///
		/// \param size_type = String
		/// \param size_type = String
		void set_selection_range(std::string::size_type start, std::string::size_type end);

		/// \brief Set selection colors
		///
		/// \param foreground = Colorf
		/// \param background = Colorf
		void set_selection_colors(const Colorf &foreground, const Colorf &background);

		/// \brief Shows the cursor caret
		void show_cursor();

		/// \brief Hides the cursor caret
		void hide_cursor();

		/// \brief Sets the cursor position
		///
		/// \param pos = Index of cursor
		void set_cursor_pos(std::string::size_type pos);

		/// \brief Toggles whether the cursor caret is shown as a solid box or a line
		///
		/// \param enable = Shows the cursor as a solid box if set to true
		void set_cursor_overwrite_mode(bool enable);

		/// \brief Sets the cursor color
		///
		/// \param color = Color of cursor
		void set_cursor_color(const Colorf &color);

		/// \brief Get Combined text
		///
		/// \return combined_text
		std::string get_combined_text() const;

		/// \brief Sets the text alignment
		///
		/// Alignment is applied when layout() is called
		///
		/// \param align = The alignment
		void set_align(SpanAlign align);

		/// \brief Returns the baseline offset for the first baseline
		int get_first_baseline_offset();

		/// \brief Returns the baseline offset for the last baseline
		int get_last_baseline_offset();

	private:
		void add_component_helper(SpanComponent *component, int baseline_offset, int id);

		std::shared_ptr<SpanLayout_Impl> impl;
	};

	/// \}
}
