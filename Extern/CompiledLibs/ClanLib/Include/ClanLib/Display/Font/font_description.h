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
**    Mark Page
**    Harry Storbacka
*/

#pragma once

#include <memory>

namespace clan
{
	/// \addtogroup clanDisplay_Font clanDisplay Font
	/// \{

	class DisplayWindow;
	class FontDescription_Impl;

	enum class FontWeight : int
	{
		thin = 100,
		extra_light = 200,
		light = 300,
		normal = 400,
		medium = 500,
		semi_bold = 600,
		bold = 700,
		extra_bold = 800,
		heavy = 900
	};

	enum class FontStyle
	{
		normal,
		italic,
		oblique	// Currently not supported by ClanLib
	};

	/// \brief Font description class.
	///
	///  This class allows you to setup a more advanced description when creating a font.
	class FontDescription
	{
	public:
		/// \brief Constructs a font description with default values.
		FontDescription();

		virtual ~FontDescription();

		/// \brief Create null object
		///
		/// \return Font Description
		static FontDescription create_null_object();

		enum Charset
		{
			charset_default,
			charset_ansi,
			charset_baltic,
			charset_chinesebig5,
			charset_easteurope,
			charset_gb2312,
			charset_greek,
			charset_hangul,
			charset_mac,
			charset_oem,
			charset_russian,
			charset_shiftjis,
			charset_symbol,
			charset_turkish,
			charset_vietnamese,
			charset_johab,
			charset_arabic,
			charset_hebrew,
			charset_thai
		};

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the font height
		float get_height() const;

		/// \brief Returns the distance between each line
		float get_line_height() const;

		/// \brief Returns the font average width
		///
		/// 0.0f = Use default
		float get_average_width() const;

		/// \brief Returns the font escapement
		float get_escapement() const;

		/// \brief Returns the font orientation
		float get_orientation() const;

		/// \brief Returns the font weight
		FontWeight get_weight() const;

		/// \brief Returns the font style
		FontStyle get_style() const;

		/// \brief Get the font anti-alias setting (defaults to true)
		bool get_anti_alias() const;

		/// \brief Get the font subpixel rendering setting (defaults to true)
		bool get_subpixel() const;

		/// \biref Get the font charset
		Charset get_charset() const;

		/// \brief Returns true if the font is identical
		/// Line_height is excluded.
		bool operator==(const FontDescription &other) const;

		/// \brief Returns an unique string identifying this font description
		///
		/// This is useful for placing font descriptions in a map.
		/// Line_height is excluded.
		std::string get_unique_id() const;

		/// \brief Copy assignment operator (does not copy the description, use clone() if you want that)
		FontDescription &operator =(const FontDescription &copy);

		// \brief Copy the entire font description (not just the implementation)
		FontDescription clone() const;

		/// \brief Sets the font height
		void set_height(float value = 20.0f);

		/// \brief Sets the font average width
		///
		/// 0.0f = Use default
		void set_average_width(float value = 0.0f);

		/// \brief Sets the font escapement
		void set_escapement(float value);

		/// \brief Sets the font orientation
		void set_orientation(float value);

		/// \brief Sets the font weight
		void set_weight(FontWeight value = FontWeight::normal);

		/// \brief Sets the distance between each line
		void set_line_height(float height);

		/// \brief Sets the font style
		void set_style(FontStyle setting = FontStyle::normal);

		/// \brief Sets the font anti-alias setting (defaults to true)
		void set_anti_alias(bool setting = true);

		/// \brief Sets the font subpixel rendering setting (defaults to true)
		void set_subpixel(bool setting = true);

		/// \brief Sets the font charset (defaults to charset_default)
		///
		/// \param new_charset = The charset. charset_default = Use operating systems default
		void set_charset(Charset new_charset);

	private:
		std::shared_ptr<FontDescription_Impl> impl;
	};

	/// \}
}
