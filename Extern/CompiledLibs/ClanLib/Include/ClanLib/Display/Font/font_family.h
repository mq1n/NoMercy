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
	/// \addtogroup clanDisplay_Font clanDisplay FontFamily
	/// \{

	class FontProvider;
	class Canvas;
	class FontFamily_Impl;
	class GlyphMetrics;

	/// \brief FontFamily class
	///
	/// A FontFamily is a collection of font descriptions
	class FontFamily
	{
	public:
		/// \brief Constructs a null font family
		FontFamily();

		/// \brief Constructs a font family with the given family name
		FontFamily(const std::string &family_name);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Font family name used for this font family
		const std::string &get_family_name() const;

		/// \brief Add standard font
		void add(const std::string &typeface_name, float height);

		// \brief Add standard font
		void add(const std::string &typeface_name, const FontDescription &desc);

		// \brief Add standard font
		void add(const FontDescription &desc, const std::string &ttf_filename);

		// \brief Add standard font
		void add(const FontDescription &desc, const std::string &ttf_filename, FileSystem fs);

		/// \brief Add a Font based on a sprite
		///
		/// \param sprite = Sprite with glyphs
		/// \param glyph_list = Letter to glyph mapping
		/// \param spacelen = Width of space character
		/// \param monospace = Force monospaced font (using widest sprite character)
		/// \param metrics = Font metrics for the sprite font
		void add(Canvas &canvas, Sprite &sprite, const std::string &glyph_list, float spacelen, bool monospace, const FontMetrics &metrics);

	private:
		std::shared_ptr<FontFamily_Impl> impl;

		friend class Font_Impl;
	};

	/// \}
}
