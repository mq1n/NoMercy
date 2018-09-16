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
#include <string>
#include <vector>
#include "../../Display/2D/color.h"
#include "style_get_value.h"
#include "style_set_value.h"
#include "style_set_image.h"
#include "style_token.h"
#include "style_tokenizer.h"

namespace clan
{
	enum class StyleDimension;
	class StyleGetValue;
	class StyleSetValue;
	class ImageSource;
	class StyleToken;

	/// Interface used by style parsers to store values in a style property set
	class StylePropertySetter
	{
	public:
		virtual ~StylePropertySetter() { }

		/// Set the value for the specified property name
		virtual void set_value(const std::string &name, const StyleSetValue &value) = 0;

		/// Set a value array for the specified property name
		virtual void set_value_array(const std::string &name, const std::vector<StyleSetValue> &value_array) = 0;
	};

	/// Parser interface used during property parsing
	class StyleParser
	{
	public:
		/// Tokenized style string relevant for parsing a property
		std::vector<StyleToken> tokens;

		/// True if the !important flag was specified in the style string
		bool important_flag = false;
	};

	/// Style property parser
	class StylePropertyParser
	{
	public:
		/// Constructs a property parser for the specified property names
		StylePropertyParser(const std::vector<std::string> &property_names);
		virtual ~StylePropertyParser() { }

		/// Parse a property and set property values if successful
		virtual void parse(StylePropertySetter *setter, const std::string &name, StyleParser &parser) = 0;

	protected:
		static StyleToken next_token(size_t &pos, const std::vector<StyleToken> &tokens, bool skip_whitespace = true);
		static bool is_length(const StyleToken &token);
		static bool is_angle(const StyleToken &token);
		static bool is_time(const StyleToken &token);
		static bool is_frequency(const StyleToken &token);
		static bool is_resolution(const StyleToken &token);
		static bool parse_length(const StyleToken &token, StyleSetValue &out_length);
		static bool parse_angle(const StyleToken &token, StyleSetValue &out_angle);
		static bool parse_time(const StyleToken &token, StyleSetValue &out_time);
		static bool parse_frequency(const StyleToken &token, StyleSetValue &out_frequency);
		static bool parse_resolution(const StyleToken &token, StyleSetValue &out_resolution);
		static bool parse_integer(const std::string &value, int &out_int);
		static bool parse_gradient(const std::vector<StyleToken> &tokens, size_t &in_out_pos, StyleGradient &out_gradient);
		static bool parse_color(const std::vector<StyleToken> &tokens, size_t &in_out_pos, Colorf &out_color);
		static bool parse_position(const std::vector<StyleToken> &tokens, size_t &in_out_pos, StyleSetValue &out_position_x, StyleSetValue &out_position_y);
		static bool equals(const std::string &s1, const std::string &s2);
		static void debug_parse_error(const std::string &name, const std::vector<StyleToken> &tokens);

	private:
	};

	/// Sets the default value for a property
	class StylePropertyDefault
	{
	public:
		StylePropertyDefault(const std::string &name, const StyleGetValue &value, bool inherit);
	};

	/// Style property interface used to parse or query properties by name
	class StyleProperty
	{
	public:
		/// Gets the default value for a given property
		static const StyleGetValue &default_value(const char *name);
		static const StyleGetValue &default_value(const std::string &name);

		/// Indicates if this an inherited property or not
		static bool is_inherited(const char *name);
		static bool is_inherited(const std::string &name);

		/// Parses a string of styles and sets the values
		static void parse(StylePropertySetter *setter, const std::string &styles);
	};
}
