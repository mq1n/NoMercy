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

#include <string>
#include "../../Display/2D/color.h"
#include "style_value_type.h"
#include "style_dimension.h"

namespace clan
{
	/// Style value variable
	class StyleSetValue
	{
	public:
		/// Variant type
		StyleValueType type = StyleValueType::undefined;

		/// Text when the type is a text string
		std::string text;

		/// Value number
		float number = 0.0f;

		/// Dimension used by value
		StyleDimension dimension = StyleDimension::px;

		/// Value color
		Colorf color;

		/// Check if value is undefined
		bool is_undefined() const { return type == StyleValueType::undefined; }

		/// Check if value is a keyword
		bool is_keyword() const { return type == StyleValueType::keyword; }
		bool is_keyword(const char *keyword) const { return is_keyword() && text == keyword; }
		bool is_keyword(const std::string &keyword) const { return is_keyword() && text == keyword; }

		/// Check if value is a length
		bool is_length() const { return type == StyleValueType::length; }

		/// Check if value is an angle
		bool is_angle() const { return type == StyleValueType::angle; }

		/// Check if value is a time
		bool is_time() const { return type == StyleValueType::time; }

		/// Check if value is a frequency
		bool is_frequency() const { return type == StyleValueType::frequency; }

		/// Check if value is a resolution
		bool is_resolution() const { return type == StyleValueType::resolution; }

		/// Check if value is a percentage
		bool is_percentage() const { return type == StyleValueType::percentage; }

		/// Check if value is a number
		bool is_number() const { return type == StyleValueType::number; }

		/// Check if value is a string
		bool is_string() const { return type == StyleValueType::string; }

		/// Check if value is an url
		bool is_url() const { return type == StyleValueType::url; }

		/// Check if value is a color
		bool is_color() const { return type == StyleValueType::color; }

		/// Create style value from a keyword
		static StyleSetValue from_keyword(const std::string &keyword) { StyleSetValue v; v.type = StyleValueType::keyword; v.text = keyword; return v; }

		/// Create style value from a string
		static StyleSetValue from_string(const std::string &text) { StyleSetValue v; v.type = StyleValueType::string; v.text = text; return v; }

		/// Create style value from a length
		static StyleSetValue from_length(float length, StyleDimension dimension = StyleDimension::px) { StyleSetValue v; v.type = StyleValueType::length; v.number = length; v.dimension = dimension; return v; }

		/// Create style value from an angle
		static StyleSetValue from_angle(float angle, StyleDimension dimension = StyleDimension::rad) { StyleSetValue v; v.type = StyleValueType::angle; v.number = angle; v.dimension = dimension; return v; }

		/// Create style value from a time
		static StyleSetValue from_time(float t, StyleDimension dimension = StyleDimension::s) { StyleSetValue v; v.type = StyleValueType::time; v.number = t; v.dimension = dimension; return v; }

		/// Create style value from a frequency
		static StyleSetValue from_frequency(float freq, StyleDimension dimension = StyleDimension::hz) { StyleSetValue v; v.type = StyleValueType::frequency; v.number = freq; v.dimension = dimension; return v; }

		/// Create style value from a resolution
		static StyleSetValue from_resolution(float resolution, StyleDimension dimension = StyleDimension::dppx) { StyleSetValue v; v.type = StyleValueType::resolution; v.number = resolution; v.dimension = dimension; return v; }

		/// Create style value from a percentage
		static StyleSetValue from_percentage(float percentage) { StyleSetValue v; v.type = StyleValueType::percentage; v.number = percentage; return v; }

		/// Create style value from a number
		static StyleSetValue from_number(float number) { StyleSetValue v; v.type = StyleValueType::number; v.number = number; return v; }

		/// Create style value from an url
		static StyleSetValue from_url(const std::string &url) { StyleSetValue v; v.type = StyleValueType::url; v.text = url; return v; }

		/// Create style value from a color
		static StyleSetValue from_color(const Colorf &color) { StyleSetValue v; v.type = StyleValueType::color; v.color = color; return v; }
	};
}
