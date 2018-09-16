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
**    Harry Storbacka
**    Kenneth Gangstoe
**    Mark Page
*/

#pragma once

#include "../Render/graphic_context.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Canvas_Impl;
	class GraphicContext;
	class PrimitivesArray;
	class Sprite_Impl;
	class Image;
	class GlyphCache;
	class Draw;
	class RenderBatcher;
	class Colorf;
	class Pointf;
	class Gradient;
	class LineSegment2f;
	class LineSegment2;
	class Trianglef;
	class Triangle;
	class DisplayWindow;
	class DisplayWindowDescription;
	class Path;
	class Pen;
	class Brush;

	/// \brief Mapping modes.
	enum MapMode
	{
		map_2d_upper_left,
		map_2d_lower_left,
		map_user_projection
	};

	/// \brief 2D Graphics Canvas
	class Canvas
	{
	public:
		/// \brief Constructs a null instance.
		Canvas();

		/// \brief Constructs a Canvas based on a framebuffer.  (based on the copy of the canvas)
		Canvas(Canvas &canvas, FrameBuffer &framebuffer);

		/// \brief Constructs a Canvas
		explicit Canvas(DisplayWindow &window);

		~Canvas();

		/// \brief Create a copy of a canvas
		Canvas create();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Get gc
		///
		/// \return Graphic Context
		GraphicContext& get_gc() const;

		/// \brief Returns the current effective transform matrix.
		const Mat4f &get_transform() const;

		/// \brief Returns the inverse of the current effective transform matrix
		///
		/// This is cached
		Mat4f &get_inverse_transform();

		/// \brief Returns the current effective projection matrix.
		const Mat4f &get_projection() const;

		operator GraphicContext&() const { return get_gc(); }

		/// \brief Returns the current width of the context.
		inline float get_width() const { return get_gc().get_dip_width(); }

		/// \brief Returns the current height of the context.
		inline float get_height() const { return get_gc().get_dip_height(); }

		/// \brief Returns the current size of the context.
		inline Sizef get_size() const { return get_gc().get_dip_size(); }

		/// \brief Returns the current clipping rectangle
		Rectf get_cliprect() const;

		/// \brief Return the content of the read buffer into a pixel buffer.
		PixelBuffer get_pixeldata(const Rect& rect, TextureFormat texture_format = tf_rgba8, bool clamp = true);

		/// \brief Return the content of the read buffer into a pixel buffer.
		PixelBuffer get_pixeldata(TextureFormat texture_format = tf_rgba8, bool clamp = true);

		/// Retrieves the display pixel ratio of the context.
		/// \seealso Resolution Independence
		float get_pixel_ratio() const { return get_gc().get_pixel_ratio(); }

		/// \brief Set active rasterizer state
		void set_rasterizer_state(const RasterizerState &state);

		/// \brief Set active blend state
		void set_blend_state(const BlendState &state, const Colorf &blend_color = StandardColorf::white(), unsigned int sample_mask = 0xffffffff);

		/// \brief Set active depth stencil state
		void set_depth_stencil_state(const DepthStencilState &state, int stencil_ref = 0);

		/// \brief Set active rasterizer state
		void reset_rasterizer_state();

		/// \brief Set active blend state
		void reset_blend_state();

		/// \brief Set active depth stencil state
		void reset_depth_stencil_state();

		/// \brief Set the current clipping rectangle.
		void set_cliprect(const Rectf &rect);

		/// \brief Push current clipping rectangle to stack.
		/** <p>If a rectangle is passed, it afterwards sets clipping
			rectangle to the union of the current rectangle and the passed
			rectangle.</p>*/
		void push_cliprect(const Rectf &rect);

		/// \brief Push cliprect
		void push_cliprect();

		/// \brief Pop current clipping rectangle from the stack.
		void pop_cliprect();

		/// \brief Removes the set clipping rectangle and empties the cliprect stack.
		void reset_cliprect();

		/// \brief Clears the whole context using the specified color.
		void clear(const Colorf &color = StandardColorf::black());

		/// \brief Clear the stencil buffer
		///
		/// \param value value to clear to.
		void clear_stencil(int value = 0);

		/// \brief Clear the depth buffer
		///
		/// \param value: value to clear to. Range 0.0 - 1.0.
		void clear_depth(float value = 0);

		/// \brief Set active program object to the standard program specified.
		void set_program_object(StandardProgram standard_program);

		/// \brief Set the projection mapping mode.
		void set_map_mode(MapMode mode);

		/// Set the viewport to be used in user projection map mode.
		///
		/// \param viewport = The viewport to set
		void set_viewport(const Rectf &viewport);

		/// \brief Set the projection matrix to be used in user projection map mode.
		void set_projection(const Mat4f &matrix);

		/// \brief Specifies which render batcher is to be currently active
		///
		/// If the render batcher is already active, nothing happens. If a different render batcher
		/// is currently active, it is flushed before the new batcher is made active.
		void set_batcher(RenderBatcher *batcher);

		/// \brief Sets the transform matrix to a new matrix.
		void set_transform(const Mat4f &matrix);

		/// \brief Multiplies the passed matrix onto the transform matrix.
		void mult_transform(const Mat4f &matrix);

		/// \brief Flushes the render batcher currently active.
		void flush();

		/// \brief Draw a point.
		void draw_point(float x1, float y1, const Colorf &color);

		/// \brief Point
		///
		/// \param gc = Graphic Context
		/// \param point = Pointf
		/// \param color = Colorf
		void draw_point(const Pointf &point, const Colorf &color);

		/// \brief Draw a line.
		void draw_line(float x1, float y1, float x2, float y2, const Colorf &color = StandardColorf::white());

		/// \brief Line
		///
		/// \param gc = Graphic Context
		/// \param start = Pointf
		/// \param end = Pointf
		/// \param color = Colorf
		void draw_line(const Pointf &start, const Pointf &end, const Colorf &color = StandardColorf::white());

		/// \brief Line
		///
		/// \param gc = Graphic Context
		/// \param line_segment = The line
		/// \param color = Colorf
		void draw_line(const LineSegment2f &line_segment, const Colorf &color = StandardColorf::white());

		/// \brief Lines
		void draw_lines(const Vec2f *positions, int num_vertices, const Colorf &color = StandardColorf::white());

		/// \brief Lines
		void draw_lines(const Vec2f *line_positions, const Vec2f *texture_positions, int num_vertices, const Texture2D &texture, const Colorf &line_color = StandardColorf::white());

		/// \brief Line Strip
		void draw_line_strip(const Vec2f *positions, int num_vertices, const Colorf &color = StandardColorf::white());

		/// \brief Draw a box / rectangle.
		void draw_box(float x1, float y1, float x2, float y2, const Colorf &color = StandardColorf::white());

		/// \brief Box
		///
		/// \param gc = Graphic Context
		/// \param start = Pointf
		/// \param end = Pointf
		/// \param color = Colorf
		void draw_box(const Pointf &start, const Pointf &end, const Colorf &color = StandardColorf::white());

		/// \brief Box
		///
		/// \param gc = Graphic Context
		/// \param rect = Rectf
		/// \param color = Colorf
		void draw_box(const Rectf &rect, const Colorf &color);

		/// \brief Draw a filled box / rectangle.
		void fill_rect(float x1, float y1, float x2, float y2, const Colorf &color = StandardColorf::white());

		/// \brief Fill
		///
		/// \param gc = Graphic Context
		/// \param start = Pointf
		/// \param end = Pointf
		/// \param color = Colorf
		void fill_rect(const Pointf &start, const Pointf &end, const Colorf &color = StandardColorf::white());

		/// \brief Fill
		///
		/// \param gc = Graphic Context
		/// \param rect = Rectf
		/// \param color = Colorf
		void fill_rect(const Rectf &rect, const Colorf &color);

		/// \brief Gradient fill
		///
		/// \param gc = Graphic Context
		/// \param x1 = value
		/// \param y1 = value
		/// \param x2 = value
		/// \param y2 = value
		/// \param color = Gradient
		void fill_rect(float x1, float y1, float x2, float y2, const Gradient &color);

		/// \brief Gradient fill
		///
		/// \param gc = Graphic Context
		/// \param start = Pointf
		/// \param end = Pointf
		/// \param gradient = Gradient
		void fill_rect(const Pointf &start, const Pointf &end, const Gradient &gradient);

		/// \brief Gradient fill
		///
		/// \param gc = Graphic Context
		/// \param rect = Rectf
		/// \param gradient = Gradient
		void fill_rect(const Rectf &rect, const Gradient &gradient);

		/// \brief Draw a circle.
		void fill_circle(float center_x, float center_y, float radius, const Colorf &color = StandardColorf::white());

		/// \brief Circle
		///
		/// \param gc = Graphic Context
		/// \param center = Pointf
		/// \param radius = value
		/// \param color = Colorf
		void fill_circle(const Pointf &center, float radius, const Colorf &color = StandardColorf::white());

		/// \brief Gradient circle
		///
		/// \param gc = Graphic Context
		/// \param center = Pointf
		/// \param radius = value
		/// \param gradient = Gradient
		void fill_circle(const Pointf &center, float radius, const Gradient &gradient);

		/// \brief Gradient circle
		///
		/// \param gc = Graphic Context
		/// \param center = Pointf
		/// \param centergradient = Pointf
		/// \param radius = value
		/// \param gradient = Gradient
		void fill_circle(const Pointf &center, const Pointf &centergradient, float radius, const Gradient &gradient);

		/// \brief Draw a triangle.
		void fill_triangle(const Pointf &a, const Pointf &b, const Pointf &c, const Colorf &color = StandardColorf::white());

		/// \brief Draw a triangle.
		void fill_triangle(const Trianglef &dest_triangle, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Gradient &gradient);

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, const Colorf *colors, int num_vertices);

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &triangles, const Colorf *colors);

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &triangles, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &triangles, const Gradient &gradient);

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *positions, const Vec2f *texture_positions, int num_vertices, const Texture2D &texture, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *positions, const Vec2f *texture_positions, int num_vertices, const Texture2D &texture, const Colorf *colors);

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const std::vector<Vec2f> &texture_positions, const Texture2D &texture, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const std::vector<Vec2f> &texture_positions, const Texture2D &texture, const std::vector<Colorf> &colors);

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const Texture2D &texture, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const Texture2D &texture, const Rect &texture_rect, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Texture2D &texture, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Texture2D &texture, const Rect &texture_rect, const Colorf &color = StandardColorf::white());

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const Texture2D &texture, const Gradient &gradient);

		/// \brief Draw triangles
		void fill_triangles(const std::vector<Vec2f> &positions, const Texture2D &texture, const Rect &texture_rect, const Gradient &gradient);

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Texture2D &texture, const Gradient &gradient);

		/// \brief Draw triangles
		void fill_triangles(const Vec2f *triangle_positions, int num_vertices, const Texture2D &texture, const Rect &texture_rect, const Gradient &gradient);

		/// \brief Draw a filled ellipse.
		void fill_ellipse(const Pointf &center, float radius_x, float radius_y, const Colorf &color = StandardColorf::white());

		/// \brief Draw a gradient filled ellipse.
		void fill_ellipse(const Pointf &center, float radius_x, float radius_y, const Gradient &gradient);

		/// \brief Snaps the point to the nearest pixel corner
		Pointf grid_fit(const Pointf &pos);

	private:
		std::shared_ptr<Canvas_Impl> impl;

		friend class Sprite_Impl;
		friend class Image;
		friend class Font_Impl;
		friend class Font_DrawSubPixel;
		friend class Font_DrawFlat;
		friend class Font_DrawScaled;
		friend class Path;
	};

	// Helper class to save the transform state for exception safety
	class TransformState
	{
	public:
		TransformState(Canvas *current_canvas) : canvas(current_canvas), matrix(current_canvas->get_transform()) {}
		~TransformState() { canvas->set_transform(matrix); }
		const Mat4f matrix;
	private:
		Canvas *canvas;
	};

	// Helper class to save the clip rect state for exception safety
	class ClipRectState
	{
	public:
		ClipRectState(Canvas *current_canvas) : canvas(current_canvas), cliprect(current_canvas->get_cliprect()) {}
		~ClipRectState() { (Rectf(canvas->get_size()) == cliprect) ? canvas->reset_cliprect() : canvas->set_cliprect(cliprect); }
		const Rectf cliprect;
	private:
		Canvas *canvas;
	};

	// Helper class to control the cliprect stack state for exception safety
	class ClipRectStack
	{
	public:
		ClipRectStack(Canvas *current_canvas) : canvas(current_canvas) {}
		~ClipRectStack() { for (; push_count > 0; --push_count) canvas->pop_cliprect(); }

		void push_cliprect(const Rectf &rect) { push_count++; canvas->push_cliprect(rect); }
		void push_cliprect() { push_count++; canvas->push_cliprect(); }
		void pop_cliprect() { if (push_count) { --push_count; canvas->pop_cliprect(); } }
	private:
		unsigned int push_count = 0;
		clan::Canvas *canvas;
	};

	/// \}
}
