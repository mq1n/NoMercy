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
*/

#pragma once

#include <memory>
#include "../../Core/Math/rect.h"
#include "../../Core/Math/mat4.h"
#include "../../Display/2D/color.h"

namespace clan
{
	class Font;
	class PathImpl;
	class GlyphMetrics;
	class Canvas;
	class Pen;
	class Brush;

	enum class PathFillMode
	{
		alternate,
		winding
	};

	class Path
	{
	public:
		Path();

		void set_fill_mode(PathFillMode fill_mode);

		void move_to(const Pointf &point);
		void move_to(float x, float y) { move_to(Pointf(x, y)); }
		void line_to(const Pointf &point);
		void line_to(float x, float y) { line_to(Pointf(x, y)); }
		void bezier_to(const Pointf &control, const Pointf &point);
		void bezier_to(const Pointf &control1, const Pointf &control2, const Pointf &point);
		void close();

		/// \brief Strokes a path
		void stroke(Canvas &canvas, const Pen &pen);

		/// \brief Fills a path
		void fill(Canvas &canvas, const Brush &brush);

		/// \brief First fills a path, then strokes on top
		void fill_and_stroke(Canvas &canvas, const Pen &pen, const Brush &brush);

		// \brief Copy the entire description (not just the implementation)
		Path clone() const;

		static Path rect(const Rectf &box);
		static Path rect(float x, float y, float width, float height) { return Path::rect(Rectf(x, y, Sizef(width, height))); }
		static Path line(const Pointf &start, const Pointf &end);
		static Path line(float x1, float y1, float x2, float y2) { return Path::line(Pointf(x1, y1), Pointf(x2, y2)); }

		static Path rect(const Rectf &box, const clan::Sizef &corner);

		static Path circle(float center_x, float center_y, float radius) { return Path::ellipse(Pointf(center_x, center_y), Sizef(radius, radius)); }
		static Path ellipse(float center_x, float center_y, float radius_x, float radius_y) { return Path::ellipse(Pointf(center_x, center_y), Sizef(radius_x, radius_y)); }
		static Path circle(const Pointf &center, float radius) { return Path::ellipse(center, Sizef(radius, radius)); }
		static Path ellipse(const Pointf &center, const Sizef &radius);

		// This function is to assist in debugging, it has not been decided if it will be removed. Don't use at the moment.
		static Path glyph(Canvas &canvas, Font &font, unsigned int glyph, GlyphMetrics &out_metrics);

		std::shared_ptr<PathImpl> get_impl() const { return impl; }

		/// \brief += operator to concatenate a path onto this path.
		///
		/// Useful when manually building complex paths from primitives
		void operator += (const Path& path);

		/// \brief Transform this path
		///
		/// Useful when manually building complex paths from primitives
		///
		/// \param transform = Transform matrix
		///
		/// \return reference to this object
		Path &transform_self(const Mat3f &transform);

	private:
		std::shared_ptr<PathImpl> impl;
		friend class CanvasImpl;
	};
	/// \brief + operator.
	Path operator + (const Path& v1, const Path& v2);
}
