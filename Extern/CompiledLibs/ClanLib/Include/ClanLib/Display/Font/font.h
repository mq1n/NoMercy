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
#include "../Render/graphic_context.h"
#include "../Image/pixel_buffer.h"
#include "../2D/sprite.h"
#include "font_description.h"
#include "glyph_metrics.h"

namespace clan
{
	/// \addtogroup clanDisplay_Font clanDisplay Font
	/// \{

	class FontProvider;
	class FontFamily;
	class Canvas;
	class Font_Impl;
	class GlyphMetrics;

	class FontHandle
	{
	public:
		virtual ~FontHandle() = 0;
	};

	/// \brief Font class
	///
	/// A Font is a collection of images that can be used to represent text on a screen.
	class Font
	{
	public:
		/// \brief Constructs a null font.
		Font();

		// \brief Create font using the specified font family
		Font(FontFamily &font_family, float height);

		// \brief Create font using the specified font family
		Font(FontFamily &font_family, const FontDescription &desc);

		/// \brief Constructs standard font
		Font(const std::string &typeface_name, float height);

		// \brief Constructs standard font
		Font(const std::string &typeface_name, const FontDescription &desc);

		// \brief Constructs standard font
		Font(const FontDescription &desc, const std::string &ttf_filename);

		// \brief Constructs standard font
		Font(const FontDescription &desc, const std::string &ttf_filename, FileSystem fs);

		/// \brief Constructs a Font based on a sprite
		///
		/// \param sprite = Sprite with glyphs
		/// \param glyph_list = Letter to glyph mapping
		/// \param spacelen = Width of space character
		/// \param monospace = Force monospaced font (using widest sprite character)
		/// \param metrics = Font metrics for the sprite font
		Font(Canvas &canvas, const std::string &typeface_name, Sprite &sprite, const std::string &glyph_list, float spacelen, bool monospace, const FontMetrics &metrics);

		/// \brief Retrieves a Font resource from the resource manager
		///
		/// \param canvas = Canvas
		/// \param resources = Resource manager
		/// \param id = id
		static Resource<Font> resource(Canvas &canvas, const std::string &family_name, const FontDescription &desc, const ResourceManager &resources);

		/// \brief Loads a Font from a XML resource definition
		static Font load(Canvas &canvas, const std::string &family_name, const FontDescription &reference_desc, FontFamily &font_family, const XMLResourceDocument &doc, std::function<Resource<Sprite>(Canvas &, const std::string &)> cb_get_sprite = std::function<Resource<Sprite>(Canvas &, const std::string &)>());

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Sets the font height
		void set_height(float value);

		/// \brief Sets the font weight
		void set_weight(FontWeight value = FontWeight::normal);

		/// \brief Sets the distance between each line
		void set_line_height(float height);

		/// \brief Sets the font style setting
		void set_style(FontStyle setting = FontStyle::normal);

		/// \brief Sets the threshold to determine if the font can be drawn scaled
		///
		/// All font sizes are scalable when using sprite fonts
		void set_scalable(float height_threshold = 64.0f);

		/// \brief Print text
		///
		/// \param canvas = Canvas
		/// \param position = Dest position
		/// \param text = The text to draw
		/// \param color = The text color
		void draw_text(Canvas &canvas, const Pointf &position, const std::string &text, const Colorf &color = StandardColorf::white());
		void draw_text(Canvas &canvas, float xpos, float ypos, const std::string &text, const Colorf &color = StandardColorf::white()) { draw_text(canvas, Pointf(xpos, ypos), text, color); }

		/// \brief Gets the glyph metrics
		///
		/// \param glyph = The glyph to get
		/// \return The glyph metrics
		GlyphMetrics get_metrics(Canvas &canvas, unsigned int glyph) const;

		/// \brief Measure text size
		///
		/// \param string = The text to use
		/// \return The metrics
		GlyphMetrics measure_text(Canvas &canvas, const std::string &string) const;

		/// \brief Retrieves font metrics description for the selected font.
		FontMetrics get_font_metrics(Canvas &canvas) const;

		/// \brief Retrieves clipped version of the text that will fit into a box
		///
		/// \return The string
		std::string get_clipped_text(Canvas &canvas, const Sizef &box_size, const std::string &text, const std::string &ellipsis_text = "...") const;

		/// \brief Get the character index at a specified point
		///
		/// \param canvas = Canvas
		/// \param text = The string
		/// \param point = The point
		/// \return The character index. -1 = Not at specified point
		int get_character_index(Canvas &canvas, const std::string &text, const Pointf &point) const;

		/// \brief Get the rectangles of each glyph in a string of text
		///
		/// \return A list of Rects for every glyph
		std::vector<Rectf> get_character_indices(Canvas &canvas, const std::string &text) const;

		// Finds the offset for the last visible character when clipping the head
		size_t clip_from_left(Canvas &canvas, const std::string &text, float width) const;

		// Finds the offset for the first visible character when clipping the tail
		size_t clip_from_right(Canvas &canvas, const std::string &text, float width) const;

		// Get the font description
		FontDescription get_description() const;

		/// \brief Get the font handle interface
		///
		/// For example, use auto handle = dynamic_cast<FontHandle_Win32>(font.get_handle()); if (handle) {...} to obtain a specific interface
		///
		/// \return The font handle interface
		FontHandle *get_handle(Canvas &canvas);

	private:
		std::shared_ptr<Font_Impl> impl;

		friend class Path;
	};

	#ifdef WIN32
	class FontEngine_Win32; // GCC needs to be explicitly told this is a class

	class FontHandle_Win32 : public FontHandle
	{
	public:
		/// \brief Get the font HFONT
		HFONT hfont();
	private:
		friend class FontEngine_Win32;
		FontEngine_Win32 *engine = nullptr;
	};
	#endif

	/// \}
}
